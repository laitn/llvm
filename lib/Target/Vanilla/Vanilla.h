//===-- Vanilla.h - Top-level interface for Vanilla representation --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// Vanilla back-end.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_VANILLA_H
#define TARGET_VANILLA_H

#include "MCTargetDesc/VanillaMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class VanillaTargetMachine;

  FunctionPass *createVanillaISelDag(VanillaTargetMachine &TM);
  FunctionPass *createVanillaVerifierPass(TargetMachine &TM);
  FunctionPass *createVanillaOperatorSRPass();

} // end namespace llvm;

#endif

