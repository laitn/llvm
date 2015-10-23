//===------- VanillaTargetObjectFile.cpp - Vanilla Object Info Impl -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "VanillaTargetObjectFile.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/Support/Dwarf.h"
#include "llvm/Target/TargetLowering.h"

using namespace llvm;

const MCExpr *VanillaELFTargetObjectFile::getTTypeGlobalReference(
    const GlobalValue *GV, unsigned Encoding, Mangler &Mang,
    const TargetMachine &TM, MachineModuleInfo *MMI,
    MCStreamer &Streamer) const {
  return TargetLoweringObjectFileELF::getTTypeGlobalReference(
      GV, Encoding, Mang, TM, MMI, Streamer);
}

void VanillaELFTargetObjectFile::Initialize(MCContext &Ctx, const TargetMachine &TM){
  TargetLoweringObjectFileELF::Initialize(Ctx, TM);
  
  this->TM = &TM;
}
