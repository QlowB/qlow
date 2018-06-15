#include "CodeGeneration.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>

using namespace qlow;

static llvm::LLVMContext context;

std::unique_ptr<llvm::Module> generateModule(const sem::SymbolTable<sem::Class>& classes)
{
    using llvm::Module;
    std::unique_ptr<Module> module = llvm::make_unique<Module>("qlow_module", context);

    return module;
}


void gen::generateObjectFile(const std::string& name, std::unique_ptr<llvm::Module> module,
        const sem::SymbolTable<sem::Class>& classes)
{
    using llvm::PassManager;


    llvm::verifyModule(*module);

    PassManager<llvm::Module> pm;
    //pm.add(createPrintModulePass(&outs()));
    //pm.run(module);


    const char cpu[] = "generic";
    const char features[] = "";


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


