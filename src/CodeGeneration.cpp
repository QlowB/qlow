#include "CodeGeneration.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Type.h>
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

    std::unique_ptr<Module> module = llvm::make_unique<Module>("qlow_module", context);
    
    std::vector<Type*> doubles(1, Type::getDoubleTy(context));
    for (auto& [name, cl] : classes){
        for (auto& [name, method] : cl->methods) {

            FunctionGenerator fg(*method, module.get());
            fg.generate();

            /*
            FunctionType* funcType = FunctionType::get(Type::getDoubleTy(context), doubles, false);
            Function* func = Function::Create(funcType, Function::ExternalLinkage, "qlow_function", module.get());
            BasicBlock* bb = BasicBlock::Create(context, "entry", func);
            IRBuilder<> builder(context);
            builder.SetInsertPoint(bb);

            Function::arg_iterator args = func->arg_begin();

            Argument* arg = &(*args);
            
            Value* val = llvm::ConstantFP::get(context, llvm::APFloat(5.0));
            Value* val2 = llvm::ConstantFP::get(context, llvm::APFloat(1.0));
            Value* result = builder.CreateFAdd(arg, val2, "add_constants");
            builder.CreateRet(result);
            */
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

    //pm.add(createPrintModulePass(&outs()));
    //pm.run(module);




    /*
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto targetMachine =
        target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

        */
/*  TargetOptions opt;
  auto RM = Optional<Reloc::Model>();
  auto TheTargetMachine =
      Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

  TheModule->setDataLayout(TheTargetMachine->createDataLayout());

  auto Filename = "output.o";
  std::error_code EC;
  raw_fd_ostream dest(Filename, EC, sys::fs::F_None);

  if (EC) {
    errs() << "Could not open file: " << EC.message();
    return 1;
  }

  legacy::PassManager pass;
  auto FileType = TargetMachine::CGFT_ObjectFile;

  if (TheTargetMachine->addPassesToEmitFile(pass, dest, llvm::LLVMTargetMachine::CGFT_ObjectFile, FileType)) {
    errs() << "TheTargetMachine can't emit a file of this type";
    return 1;
  }

  pass.run(*TheModule);
  dest.flush();

  outs() << "Wrote " << Filename << "\n";

  return 0;*/
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

    std::vector<Type*> doubles(1, Type::getDoubleTy(context));
    FunctionType* funcType = FunctionType::get(Type::getDoubleTy(context), doubles, false);
    Function* func = Function::Create(funcType, Function::ExternalLinkage, method.name, module);
    BasicBlock* bb = BasicBlock::Create(context, "entry", func);
    
    /*Function::arg_iterator args = func->arg_begin();

    Argument* arg = &(*args);
    
    Value* val = llvm::ConstantFP::get(context, llvm::APFloat(5.0));
    Value* val2 = llvm::ConstantFP::get(context, llvm::APFloat(1.0));
    Value* result = builder.CreateFAdd(arg, val2, "add_constants");
    builder.CreateRet(result);
    */

    pushBlock(bb);

    IRBuilder<> builder(context);
    builder.SetInsertPoint(bb);
    for (auto& var : method.body->variables) {
        llvm::AllocaInst* v = builder.CreateAlloca(Type::getDoubleTy(context));
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
    builder.CreateRetVoid();

    return func;
}



