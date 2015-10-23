//===-- VanillaBaseInfo.h - Top level definitions for VANILLA MC ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the Mips target useful for the compiler back-end and the MC libraries.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_VANILLA_MCTARGETDESC_VANILLABASEINFO_H
#define LLVM_LIB_TARGET_VANILLA_MCTARGETDESC_VANILLABASEINFO_H

#include "VanillaFixupKinds.h"
#include "VanillaMCTargetDesc.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {
  
  /// VanillaII - This namespace holds all of the target specific flags that
  /// instruction info tracks.
  ///
  namespace VanillaII {
    /// Target Operand Flag enum.
    enum TOF {
      MO_NO_FLAG,
      MO_NO_LO11
    };
  }
}

#endif
