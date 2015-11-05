//===-- Approx.cpp -------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements common infrastructure for libLLVMApprox.a, which
// implements approximate transformations over the LLVM intermediate.
//
//===----------------------------------------------------------------------===//

#include "llvm-c/Core.h"
#include "llvm-c/Initialization.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/IR/LegacyPassManager.h"

namespace llvm {
  class PassRegistry;
}

using namespace llvm;

/// initializeApproximation - Initialize all passes linked into the
/// Approx library.
void llvm::initializeApproximation(PassRegistry &Registry) {
  initializeOSRPass(Registry);
  initializeDESELECTPass(Registry);
}

void LLVMInitializeApproximation(LLVMPassRegistryRef R) {
  initializeApproximation(*unwrap(R));
}

