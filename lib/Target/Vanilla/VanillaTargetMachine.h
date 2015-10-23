//===-- VanillaTargetMachine.h - Define TargetMachine for Vanilla --- C++ ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Vanilla specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Vanilla_VanillaTARGETMACHINE_H
#define LLVM_LIB_TARGET_Vanilla_VanillaTARGETMACHINE_H

#include "VanillaSubtarget.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class VanillaTargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  VanillaSubtarget Subtarget;

public:
  VanillaTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options, Reloc::Model RM,
                   CodeModel::Model CM, CodeGenOpt::Level OL);

  const VanillaSubtarget *getSubtargetImpl() const { return &Subtarget; }
  const VanillaSubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
};
}

#endif

