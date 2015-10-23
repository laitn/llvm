//===-- VanillaTargetObjectFile.h - Vanilla Object Info -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VANILLA_VANILLATARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_VANILLA_VANILLATARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

class MCContext;
class TargetMachine;

class VanillaELFTargetObjectFile : public TargetLoweringObjectFileELF {
   const TargetMachine *TM;
public:
  VanillaELFTargetObjectFile() :
    TargetLoweringObjectFileELF()
  {}
  
  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;

  const MCExpr *
  getTTypeGlobalReference(const GlobalValue *GV, unsigned Encoding,
                          Mangler &Mang, const TargetMachine &TM,
                          MachineModuleInfo *MMI,
                          MCStreamer &Streamer) const override;
};

} // end namespace llvm

#endif

