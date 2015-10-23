//===-- VanillaTargetInfo.cpp - Vanilla Target Implementation -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Vanilla.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheVanillaTarget;

extern "C" void LLVMInitializeVanillaTargetInfo() {
  RegisterTarget<Triple::vanilla, /*HasJIT=*/true> X(TheVanillaTarget, "vanilla", "Vanilla");

}
