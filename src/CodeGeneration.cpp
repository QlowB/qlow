#include "CodeGeneration.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>


using namespace qlow;

static llvm::LLVMContext context;

namespace qlow
{
namespace gen
{

std::unique_ptr<llvm::Module> generateModule(const sem::SymbolTable<sem::Class>& classes)
{
    using llvm::Module;
    using llvm::Function;
    using llvm::Argument;
    using llvm::Type;
    using llvm::FunctionType;
    using llvm::BasicBlock;
    using llvm::Value;
    using llvm::IRBuilder;
    
#ifdef DEBUGGING
        printf("creating llvm module\n"); 
#endif 

    std::unique_ptr<Module> module = llvm::make_unique<Module>("qlow_module", context);

    // create llvm structs
    // TODO implement detection of circles
    for (auto& [name, cl] : classes){
        llvm::StructType* st;
        std::vector<llvm::Type*> fields;
#ifdef DEBUGGING
        printf("creating llvm struct for %s\n", name.c_str());
#endif 
        for (auto& [name, field] : cl->fields) {
            fields.push_back(field->type.getLlvmType(context));
            if (fields[fields.size() - 1] == nullptr)
                throw "internal error: possible circular dependency";
        }
        st = llvm::StructType::create(context, fields, name);
        cl->llvmType = st;
    }
    
    // create all llvm functions
    for (auto& [name, cl] : classes) {
        for (auto& [name, method] : cl->methods) {
            std::vector<Type*> argumentTypes;
            Type* returnType = method->returnType.getLlvmType(context);
            for (auto& arg : method->arguments) {
                Type* argumentType = arg->type.getLlvmType(context);
                argumentTypes.push_back(argumentType);
            }
            
            FunctionType* funcType = FunctionType::get(
                returnType, argumentTypes, false);
#ifdef DEBUGGING
            printf("looking up llvm type of %s\n", name.c_str());
#endif 
            if (returnType == nullptr)
                throw "invalid return type";
            Function* func = Function::Create(funcType, Function::ExternalLinkage, method->name, module.get());
            method->llvmNode = func;
            size_t index = 0;
            for (auto& arg : func->args()) {
                method->arguments[index]->allocaInst = &arg;
#ifdef DEBUGGING
                printf("allocaInst of arg '%s': %p\n", method->arguments[index]->name.c_str(), method->arguments[index]->allocaInst);
#endif 
                index++;
            }
        }
    }

    for (auto& [name, cl] : classes){
        for (auto& [name, method] : cl->methods) {
            FunctionGenerator fg(*method, module.get());
            Function* f = fg.generate();
            llvm::verifyFunction(*f, &llvm::errs());
#ifdef DEBUGGING
            printf("verified function: %s\n", method->name.c_str());
#endif
        }
    }
    return module;
}


void generateObjectFile(const std::string& filename, std::unique_ptr<llvm::Module> module)
{
    using llvm::legacy::PassManager;
    using llvm::raw_fd_ostream;
    using llvm::Target;
    using llvm::TargetMachine;
    using llvm::TargetRegistry;
    using llvm::TargetOptions;


    printf("verifying mod\n");
    module->print(llvm::errs(), nullptr);
    llvm::verifyModule(*module);
    printf("mod verified\n");


    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    PassManager pm;

    const char cpu[] = "generic";
    const char features[] = "";

    std::string error;
    std::string targetTriple = llvm::sys::getDefaultTargetTriple();
    const Target* target = TargetRegistry::lookupTarget(targetTriple, error);

    if (!target)
        fprintf(stderr, "could not create target: %s", error.c_str());

    TargetOptions targetOptions;
    auto relocModel = llvm::Optional<llvm::Reloc::Model>();
    TargetMachine* targetMachine = target->createTargetMachine(targetTriple, cpu,
            features, targetOptions, relocModel);

    std::error_code errorCode;
    raw_fd_ostream dest(filename, errorCode, llvm::sys::fs::F_None);
    targetMachine->addPassesToEmitFile(pm, dest, llvm::LLVMTargetMachine::CGFT_ObjectFile,
            llvm::TargetMachine::CGFT_ObjectFile);

    pm.run(*module);
    dest.flush();
    dest.close();

    return;
}

} // namespace gen
} // namespace qlow


llvm::Function* qlow::gen::FunctionGenerator::generate(void)
{
    using llvm::Function;
    using llvm::Argument;
    using llvm::Type;
    using llvm::FunctionType;
    using llvm::BasicBlock;
    using llvm::Value;
    using llvm::IRBuilder;
    
#ifdef DEBUGGING
    printf("generate function %s\n", method.name.c_str()); 
#endif

    Function* func = module->getFunction(method.name);

    if (func == nullptr) {
        throw "internal error: function not found";
    }

    BasicBlock* bb = BasicBlock::Create(context, "entry", func);

    pushBlock(bb);

    IRBuilder<> builder(context);
    builder.SetInsertPoint(bb);
    for (auto& [name, var] : method.body->scope.getLocals()) {
        if (var.get() == nullptr)
            throw "wtf null variable";
        llvm::AllocaInst* v = builder.CreateAlloca(var->type.getLlvmType(context));
        var->allocaInst = v;
    }
    
    for (auto& statement : method.body->statements) {
#ifdef DEBUGGING
        printf("statement visit %s\n", statement->toString().c_str());
#endif
        statement->accept(statementVisitor, *this);
    }
    

#ifdef DEBUGGING
    printf("End of Function\n");
#endif
    
    //Value* val = llvm::ConstantFP::get(context, llvm::APFloat(5.0));
    
    builder.SetInsertPoint(getCurrentBlock());
    builder.CreateRetVoid();

    return func;
}



