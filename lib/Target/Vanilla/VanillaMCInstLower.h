//===-- VanillaMCInstLower.h - Lower MachineInstr to MCInst ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VANILLA_VANILLAMCINSTLOWER_H
#define LLVM_LIB_TARGET_VANILLA_VANILLAMCINSTLOWER_H

#include "llvm/Support/Compiler.h"

namespace llvm {
    class AsmPrinter;
    class MCContext;
    class MCInst;
    class MCOperand;
    class MCSymbol;
    class MachineInstr;
    class MachineFunction;
    class MachineOperand;
    class VanillaAsmPrinter;
    
    // VanillaMCInstLower - This class is used to lower an MachineInstr into an MCInst.
    class LLVM_LIBRARY_VISIBILITY VanillaMCInstLower {
        MCContext &Ctx;
        
        AsmPrinter &Printer;
        
    public:
        VanillaMCInstLower(MCContext &ctx, AsmPrinter &printer)
        : Ctx(ctx), Printer(printer) {}
        void Lower(const MachineInstr *MI, MCInst &OutMI) const;
        
        MCOperand LowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;
        
        MCSymbol *GetGlobalAddressSymbol(const MachineOperand &MO) const;
    };
}

#endif
