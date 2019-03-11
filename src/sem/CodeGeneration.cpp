#include "CodeGeneration.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Attributes.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_os_ostream.h>


using namespace qlow;

static llvm::LLVMContext context;

namespace qlow
{
namespace gen
{

std::unique_ptr<llvm::Module> generateModule(const sem::Semantic& semantic)
{
    using llvm::Module;
    using llvm::Function;
    using llvm::Argument;
    using llvm::Type;
    using llvm::FunctionType;
    using llvm::BasicBlock;
    using llvm::Value;
    using llvm::IRBuilder;
    
    Logger& logger = Logger::getInstance();
    
#ifdef DEBUGGING
        printf("creating llvm module\n"); 
#endif 

    std::unique_ptr<Module> module = llvm::make_unique<Module>("qlow_module", context);

    // create llvm structs
    // TODO implement detection of circles
    for (auto& [name, cl] : semantic.getClasses()) {
        llvm::StructType* st;
        std::vector<llvm::Type*> fields;
#ifdef DEBUGGING
        printf("creating llvm struct for %s\n", name.c_str());
#endif
        int llvmStructIndex = 0;
        for (auto& [name, field] : cl->fields) {
            field->llvmStructIndex = llvmStructIndex;
            fields.push_back(field->type->getLlvmType(context));
            if (fields[fields.size() - 1] == nullptr)
                throw "internal error: possible circular dependency";
            
            llvmStructIndex++;
        }
        st = llvm::StructType::create(context, fields, name);
        cl->llvmType = st;
    }
    
    llvm::AttrBuilder ab;
    ab.addAttribute(llvm::Attribute::AttrKind::NoInline);
    ab.addAttribute(llvm::Attribute::AttrKind::NoUnwind);
    //ab.addAttribute(llvm::Attribute::AttrKind::OptimizeNone);
    //ab.addAttribute(llvm::Attribute::AttrKind::UWTable);
    ab.addAttribute("no-frame-pointer-elim", "true");
    ab.addAttribute("no-frame-pointer-elim-non-leaf");
    llvm::AttributeSet as = llvm::AttributeSet::get(context, ab);
    
    
    std::vector<llvm::Function*> functions;
    auto verifyStream = llvm::raw_os_ostream(logger.debug());
    
    // create all llvm functions
    for (auto& [name, cl] : objects.classes) {
        for (auto& [name, method] : cl->methods) {
            Function* func = generateFunction(module.get(), method.get());
            for (auto a : as) {
                func->addFnAttr(a);
            }
            functions.push_back(func);
        }
    }
    
    for (auto& [name, method] : objects.functions) {
        Function* func = generateFunction(module.get(), method.get());
        for (auto a : as) {
            func->addFnAttr(a);
        }
        functions.push_back(func);
    }

    for (auto& [name, cl] : objects.classes){
        for (auto& [name, method] : cl->methods) {
            if (!method->body)
                continue;
            
            FunctionGenerator fg(*method, module.get(), as);
            Function* f = fg.generate();
            logger.debug() << "verifying function: " << method->name << std::endl;
            bool corrupt = llvm::verifyFunction(*f, &verifyStream);
            if (corrupt) {
                module->print(verifyStream, nullptr);
                throw "corrupt llvm function";
            }
#ifdef DEBUGGING
            printf("verified function: %s\n", method->name.c_str());
#endif
        }
    }
    for (auto& [name, method] : objects.functions) {
        if (!method->body)
            continue;
        
        FunctionGenerator fg(*method, module.get(), as);
        Function* f = fg.generate();
        logger.debug() << "verifying function: " << method->name << std::endl;
        bool corrupt = llvm::verifyFunction(*f, &verifyStream);
        if (corrupt) {
            module->print(verifyStream, nullptr);
            throw "corrupt llvm function";
        }
#ifdef DEBUGGING
        printf("verified function: %s\n", method->name.c_str());
#endif
    }
    return module;
}


llvm::Function* generateFunction(llvm::Module* module, sem::Method* method)
{
    using llvm::Function;
    using llvm::Argument;
    using llvm::Type;
    using llvm::FunctionType;
    
    Type* returnType;
    if (method->returnType)
        returnType = method->returnType->getLlvmType(context);
    else
        returnType = llvm::Type::getVoidTy(context);
    
    std::vector<Type*> argumentTypes;
    if (method->thisExpression != nullptr) {
        Type* enclosingType = method->thisExpression->type->getLlvmType(context);
        argumentTypes.push_back(enclosingType);
    }
    
    for (auto& arg : method->arguments) {
        Type* argumentType = arg->type->getLlvmType(context);
        argumentTypes.push_back(argumentType);
    }
    
    FunctionType* funcType = FunctionType::get(
        returnType, argumentTypes, false);
#ifdef DEBUGGING
    printf("looking up llvm type of %s\n", method->name.c_str());
#endif 
    if (returnType == nullptr)
        throw "invalid return type";
    Function* func = Function::Create(funcType, Function::ExternalLinkage, method->name, module);
    method->llvmNode = func;
    
    // linking alloca instances for funcs
    auto argIterator = func->arg_begin();
    if (method->thisExpression != nullptr) {
        method->thisExpression->allocaInst = &*argIterator;
        Logger::getInstance().debug() << "allocaInst of this";
        argIterator++;
    }
    
    size_t argIndex = 0;
    for (; argIterator != func->arg_end(); argIterator++) {
        if (argIndex > method->arguments.size())
            throw "internal error";
        method->arguments[argIndex]->allocaInst = &*argIterator;
#ifdef DEBUGGING
        printf("allocaInst of arg '%s': %p\n", method->arguments[argIndex]->name.c_str(), method->arguments[argIndex]->allocaInst);
#endif 
        argIndex++;
    }
    
    //printf("UEEEEEEEE %s\n", method->name.c_str());
    return func;
}


void generateObjectFile(const std::string& filename, std::unique_ptr<llvm::Module> module, int optLevel)
{
    using llvm::legacy::PassManager;
    using llvm::PassManagerBuilder;
    using llvm::raw_fd_ostream;
    using llvm::Target;
    using llvm::TargetMachine;
    using llvm::TargetRegistry;
    using llvm::TargetOptions;

    Logger& logger = Logger::getInstance();
    logger.debug() << "verifying mod" << std::endl;
    auto ostr = llvm::raw_os_ostream(logger.debug());
#ifdef DEBUGGING
    module->print(ostr, nullptr);
#endif
    bool broken = llvm::verifyModule(*module);
    
    if (broken)
        throw "invalid llvm module";
    
    logger.debug() << "mod verified" << std::endl;

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    PassManager pm;
    
    int sizeLevel = 0;
    PassManagerBuilder builder;
    builder.OptLevel = optLevel;
    builder.SizeLevel = sizeLevel;
    if (optLevel >= 2) {
        builder.DisableUnitAtATime = false;
        builder.DisableUnrollLoops = false;
        builder.LoopVectorize = true;
        builder.SLPVectorize = true;
    }

    builder.populateModulePassManager(pm);

    const char cpu[] = "generic";
    const char features[] = "";

    std::string error;
    std::string targetTriple = llvm::sys::getDefaultTargetTriple();
    const Target* target = TargetRegistry::lookupTarget(targetTriple, error);

    if (!target) {
        logger.debug() << "could not create target: " << error << std::endl;
        throw "internal error";
    }

    TargetOptions targetOptions;
    auto relocModel = llvm::Optional<llvm::Reloc::Model>(llvm::Reloc::Model::PIC_);
    std::unique_ptr<TargetMachine> targetMachine(
        target->createTargetMachine(targetTriple, cpu,
            features, targetOptions, relocModel));

    std::error_code errorCode;
    raw_fd_ostream dest(filename, errorCode, llvm::sys::fs::F_None);
    targetMachine->addPassesToEmitFile(pm, dest,
//        llvm::LLVMTargetMachine::CGFT_ObjectFile,
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
        if (var->type == nullptr)
            throw "wtf null type";
        
        llvm::AllocaInst* v = builder.CreateAlloca(var->type->getLlvmType(context));
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
    if (method.returnType->equals(sem::NativeType(sem::NativeType::Type::VOID))) {
        if (!getCurrentBlock()->getTerminator())
            builder.CreateRetVoid();
    }

    return func;
}


