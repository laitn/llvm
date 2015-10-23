//===-- VanillaMCExpr.cpp - Vanilla specific MC expression classes --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the implementation of the assembly expression modifiers
// accepted by the Vanilla architecture (e.g. "%hi", "%lo", ...).
//
//===----------------------------------------------------------------------===//

#include "VanillaMCExpr.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCObjectStreamer.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/Object/ELF.h"


using namespace llvm;

#define DEBUG_TYPE "vanillamcexpr"

const VanillaMCExpr*
VanillaMCExpr::create(VariantKind Kind, const MCExpr *Expr,
                      MCContext &Ctx) {
    return new (Ctx) VanillaMCExpr(Kind, Expr);
}

void VanillaMCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {

  bool closeParen = printVariantKind(OS, Kind);

  const MCExpr *Expr = getSubExpr();
  Expr->print(OS, MAI);

  if (closeParen)
    OS << ')';
}

bool VanillaMCExpr::printVariantKind(raw_ostream &OS, VariantKind Kind)
{
  bool closeParen = true;
  switch (Kind) {
  case VK_Vanilla_None:     closeParen = false; break;
  case VK_Vanilla_CALL:       OS << "%call(";  break;
  case VK_Vanilla_BR6:        OS<<"%br6(";break;
  case VK_Vanilla_BR11:       OS<<"%br11(";break;
  }
  return closeParen;
}

VanillaMCExpr::VariantKind VanillaMCExpr::parseVariantKind(StringRef name)
{
  return StringSwitch<VanillaMCExpr::VariantKind>(name)
    .Case("call",  VK_Vanilla_CALL)
    .Default(VK_Vanilla_None);
}

Vanilla::Fixups VanillaMCExpr::getFixupKind(VanillaMCExpr::VariantKind Kind) {
  switch (Kind) {
  default: llvm_unreachable("Unhandled VanillaMCExpr::VariantKind");
  case VK_Vanilla_CALL:       return Vanilla::fixup_vanilla_pc_11;
  case VK_Vanilla_BR6:        return Vanilla::fixup_vanilla_pc_re6;
  case VK_Vanilla_BR11:       return Vanilla::fixup_vanilla_pc_re11;
  }
}

bool
VanillaMCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                       const MCAsmLayout *Layout,
                                       const MCFixup *Fixup) const {
  return getSubExpr()->evaluateAsRelocatable(Res, Layout, Fixup);
}

void VanillaMCExpr::fixELFSymbolsInTLSFixups(MCAssembler &Asm) const {
  switch(getKind()) {
    default: return;
  }
}

void VanillaMCExpr::visitUsedExpr(MCStreamer &Streamer) const {
  Streamer.visitUsedExpr(*getSubExpr());
}

