#include "CodeGeneration.h"

using namespace qlow;


void generateModule(const sem::SymbolTable<sem::Class>& classes)
{

}


void gen::generateObjectFile(const std::string& name, const sem::SymbolTable<sem::Class>& classes)
{
    const char cpu[] = "generic";
    const char features[] = "";

    auto targetMachine =
        target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

  TargetOptions opt;
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

  return 0;
}


