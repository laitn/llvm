//===-- VanillaMCAsmInfo.h - Vanilla asm properties -------------------*- C++ -*--====//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the VanillaMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VANILLA_MCTARGETDESC_VANILLAMCASMINFO_H
#define LLVM_LIB_TARGET_VANILLA_MCTARGETDESC_VANILLAMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
    class Target;
    class Triple;
    
    class VanillaELFMCAsmInfo : public MCAsmInfoELF {
        virtual void anchor();
    public:
        explicit VanillaELFMCAsmInfo(const Triple &TT) {
          IsLittleEndian = true;
            
          UsesELFSectionDirectiveForBSS = true;
          HasSingleParameterDotFile = false;
          HasDotTypeDotSizeDirective = false;
            
          SupportsDebugInformation = true;
        }
    };
}

#endif
