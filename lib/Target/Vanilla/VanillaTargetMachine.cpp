//===-- VanillaTargetMachine.cpp - Define TargetMachine for Vanilla -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about Vanilla target spec.
//
//===----------------------------------------------------------------------===//

#include "VanillaTargetMachine.h"
#include "VanillaTargetObjectFile.h"
#include "Vanilla.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Transforms/Approx.h"
using namespace llvm;

extern "C" void LLVMInitializeVanillaTarget() {
  RegisterTargetMachine<VanillaTargetMachine> X(TheVanillaTarget);
}

// DataLayout: little endian
static std::string computeDataLayout(const Triple &TT) {
  return "e-m:e-p:32:32-i32:32-n32:32";
}

VanillaTargetMachine::VanillaTargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   Reloc::Model RM, CodeModel::Model CM,
                                   CodeGenOpt::Level OL)
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options, RM, CM, OL),
      TLOF(make_unique<TargetLoweringObjectFileELF>()),
      Subtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

namespace {
// Vanilla Code Generator Pass Configuration Options.
class VanillaPassConfig : public TargetPassConfig {
public:
  VanillaPassConfig(VanillaTargetMachine *TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  VanillaTargetMachine &getVanillaTargetMachine() const {
    return getTM<VanillaTargetMachine>();
  }

  void addIRPasses() override;
  bool addInstSelector() override;
  void addPreEmitPass() override;
};
}

TargetPassConfig *VanillaTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new VanillaPassConfig(this, PM);
}

void VanillaPassConfig::addIRPasses(){
  addPass(createOSRPass());
  addPass(createDESELECTPass());
  TargetPassConfig::addIRPasses();
}

// Install an instruction selector pass using
// the ISelDag to gen Vanilla code.
bool VanillaPassConfig::addInstSelector() {
  addPass(createVanillaISelDag(getVanillaTargetMachine()));
  return false;
}

void VanillaPassConfig::addPreEmitPass(){
  addPass(createVanillaVerifierPass(getVanillaTargetMachine()));
}
