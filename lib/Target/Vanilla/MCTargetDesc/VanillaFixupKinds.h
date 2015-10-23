//===-- VanillaFixupKinds.h - Vanilla Specific Fixup Entries --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VANILLA_MCTARGETDESC_VANILLAFIXUPKINDS_H
#define LLVM_LIB_TARGET_VANILLA_MCTARGETDESC_VANILLAFIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
  namespace Vanilla {
    enum Fixups {
      // 11-bit PC absolute relocation for call
      fixup_vanilla_pc_11 = FirstTargetFixupKind,

      /// 6-bit PC relative relocation for
      /// branches
      fixup_vanilla_pc_re6,
      
      /// 6-bit PC relative relocation for
      /// branches
      fixup_vanilla_pc_re11,

      // Marker
      LastTargetFixupKind,
      NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
    };
  }
}

#endif
