#include "CodeGeneration.h"
#include "Mangling.h"
#include "Linking.h"

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
#include <llvm/Support/Host.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/raw_os_ostream.h>


using namespace qlow;

static llvm::LLVMContext context;

namespace qlow
{
namespace gen
{

std::unique_ptr<llvm::Module> generateModule(sem::GlobalScope& semantic)
{
    using llvm::Module;
    using llvm::Function;
    using llvm::Argument;
    using llvm::Type;
    using llvm::FunctionType;
    using llvm::BasicBlock;
    using llvm::Value;
    using llvm::IRBuilder;
    
    Printer& printer = Printer::getInstance();
    
#ifdef DEBUGGING
        printf("creating llvm module\n"); 
#endif 

    std::unique_ptr<Module> module = std::make_unique<Module>("qlow_module", context);

    // create llvm structs
    // TODO implement detection of circles
    semantic.getContext().createLlvmTypes(context);
    
    llvm::AttrBuilder ab;
    ab.addAttribute(llvm::Attribute::AttrKind::NoInline);
    ab.addAttribute(llvm::Attribute::AttrKind::NoUnwind);
    //ab.addAttribute(llvm::Attribute::AttrKind::OptimizeNone);
    //ab.addAttribute(llvm::Attribute::AttrKind::UWTable);
    ab.addAttribute("no-frame-pointer-elim", "true");
    ab.addAttribute("no-frame-pointer-elim-non-leaf");
    llvm::AttributeSet as = llvm::AttributeSet::get(context, ab);
    
    
    std::vector<llvm::Function*> functions;
    auto verifyStream = llvm::raw_os_ostream(printer);
    
    // create all llvm functions
    for (const auto& [name, cl] : semantic.getClasses()) {
        for (const auto& [name, method] : cl->methods) {
            Function* func = generateFunction(module.get(), method.get());
            for (auto a : as) {
                func->addFnAttr(a);
            }
            functions.push_back(func);
        }
    }
    
    for (const auto& [name, method] : semantic.getMethods()) {
        Function* func = generateFunction(module.get(), method.get());
        for (auto a : as) {
            func->addFnAttr(a);
        }
        functions.push_back(func);
    }

    for (const auto& [name, cl] : semantic.getClasses()){
        for (const auto& [name, method] : cl->methods) {
            if (!method->body)
                continue;
            
            FunctionGenerator fg(*method, module.get(), as);
            Function* f = fg.generate();
//            printer << "verifying function: " << method->name << std::endl;
            bool corrupt = llvm::verifyFunction(*f, &verifyStream);
            if (corrupt) {
#ifdef DEBUGGING
                //module->print(verifyStream, nullptr);
#endif
                throw (std::string("corrupt llvm function: ") + method->name).c_str();
            }
#ifdef DEBUGGING
            printf("verified function: %s\n", method->name.c_str());
#endif
        }
    }
    for (const auto& [name, method] : semantic.getMethods()) {
        if (!method->body)
            continue;
        
        FunctionGenerator fg(*method, module.get(), as);
        Function* f = fg.generate();
        //printer.debug() << "verifying function: " << method->name << std::endl;
        bool corrupt = llvm::verifyFunction(*f, &verifyStream);
        if (corrupt) {
            f->print(verifyStream, nullptr);
            throw (std::string("corrupt llvm function: ") + method->name).c_str();
        }
#ifdef DEBUGGING
        printf("verified function: %s\n", method->name.c_str());
#endif
    }
    auto mainMethod = semantic.getMethod("main");
    if (mainMethod != nullptr) {
        generateStartFunction(module.get(), mainMethod->llvmNode);
    }
    return module;
}


llvm::Function* generateFunction(llvm::Module* module, sem::Method* method)
{
    sem::Context& semCtxt = method->context;
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
    std::string symbolName = method->getMangledName();
    Function* func = Function::Create(funcType, Function::ExternalLinkage, symbolName, module);
    method->llvmNode = func;
    
    // linking alloca instances for funcs
    auto argIterator = func->arg_begin();
    if (method->thisExpression != nullptr) {
        method->thisExpression->allocaInst = &*argIterator;
#ifdef DEBUGGING
        Printer::getInstance() << "allocaInst of this";
#endif
        argIterator++;
    }
    
    size_t argIndex = 0;
    for (; argIterator != func->arg_end(); argIterator++) {
        if (argIndex > method->arguments.size())
            throw "internal error";
        method->arguments[argIndex]->allocaInst = &*argIterator;
#ifdef DEBUGGING
        printf("allocaInst of arg '%s': %p\n", method->arguments[argIndex]->name.c_str(), (void*)method->arguments[argIndex]->allocaInst);
#endif 
        argIndex++;
    }
    
    //printf("UEEEEEEEE %s\n", method->name.c_str());
    return func;
}



llvm::Function* generateStartFunction(llvm::Module* module, llvm::Function* start)
{
    using llvm::Function;
    using llvm::FunctionType;
    using llvm::Type;
    using llvm::IRBuilder;
    using llvm::BasicBlock;
    using llvm::Value;

    FunctionType* startFuncType = FunctionType::get(
        Type::getVoidTy(context), { Type::getInt32Ty(context), Type::getInt8PtrTy(context)->getPointerTo() }, false);
    FunctionType* exitFuncType = FunctionType::get(
        Type::getVoidTy(context), { Type::getInt32Ty(context) }, false);
    Function* startFunction = Function::Create(startFuncType, Function::ExternalLinkage, "_qlow_start", module);
    Function* exitFunction = Function::Create(exitFuncType, Function::ExternalLinkage, qlow::getExternalSymbol("exit"), module);


    IRBuilder<> builder(context);
    BasicBlock* bb = BasicBlock::Create(context, "entry", startFunction);
    builder.SetInsertPoint(bb);
    auto returnVal = builder.CreateCall(start, {});

    if (start->getReturnType()->isIntegerTy()) {
        auto rv = builder.CreateIntCast(returnVal, llvm::Type::getInt32Ty(context), true);
        builder.CreateCall(exitFunction, { rv });
    }
    else {
        builder.CreateCall(exitFunction, { llvm::ConstantInt::get(context, llvm::APInt(32, "0", 10)) });
    }
    builder.CreateRetVoid();

    return startFunction;
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

    Printer& printer = Printer::getInstance();
#ifdef DEBUGGING
    printer << "verifying mod" << std::endl;
#endif
    auto ostr = llvm::raw_os_ostream(printer);
#ifdef DEBUGGING
    module->print(ostr, nullptr);
#endif
    bool broken = llvm::verifyModule(*module);
    
    if (broken)
        throw "invalid llvm module";
    
    llvm::InitializeNativeTarget ();
    llvm::InitializeNativeTargetAsmPrinter();
    //llvm::InitializeAllTargetInfos();
    //llvm::InitializeAllTargets();
    //llvm::InitializeAllTargetMCs();
    //llvm::InitializeAllAsmParsers();
    //llvm::InitializeAllAsmPrinters();

    PassManager pm;
    
    int sizeLevel = 0;
    PassManagerBuilder builder;
    builder.OptLevel = optLevel;
    builder.SizeLevel = sizeLevel;
    if (optLevel >= 2) {
        //builder.DisableUnitAtATime = false;
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
#ifdef DEBUGGING
        printer << "could not create target: " << error << std::endl;
#endif
        throw "internal error";
    }

    TargetOptions targetOptions;
    auto relocModel = llvm::Optional<llvm::Reloc::Model>(llvm::Reloc::Model::PIC_);
    TargetMachine* targetMachine = target->createTargetMachine(targetTriple, cpu,
            features, targetOptions, relocModel);

    std::error_code errorCode;

    raw_fd_ostream dest(filename, errorCode, llvm::sys::fs::F_None);
#ifdef DEBUGGING
    printer << "adding passes" << std::endl;
#endif
    targetMachine->addPassesToEmitFile(pm, dest,
//        llvm::LLVMTargetMachine::CGFT_ObjectFile,
#if defined(LLVM_VERSION_MAJOR) && LLVM_VERSION_MAJOR >= 7
            nullptr,
#endif
//        nullptr,
        llvm::CGFT_ObjectFile);

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

    sem::Context& semCtxt = this->method.context;
    
#ifdef DEBUGGING
    printf("generate function %s\n", method.name.c_str()); 
#endif

    Function* func = module->getFunction(method.getMangledName());

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
    //if (method.returnType->equals(sem::NativeType(sem::NativeType::Type::VOID))) {
    if (method.returnType == nullptr || method.returnType->isVoid()) {
        builder.CreateRetVoid();
    }
    return func;
}



