//====- VanillaMCExpr.h - Vanilla specific MC expression classes --*- C++ -*-=====//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes Vanilla-specific MCExprs, used for modifiers like
// "%hi" or "%lo" etc.,
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VANILLA_MCTARGETDESC_VANILLAMCEXPR_H
#define LLVM_LIB_TARGET_VANILLA_MCTARGETDESC_VANILLAMCEXPR_H

#include "VanillaFixupKinds.h"
#include "llvm/MC/MCExpr.h"

namespace llvm {

class StringRef;
class VanillaMCExpr : public MCTargetExpr {
public:
  enum VariantKind {
    VK_Vanilla_None,
    VK_Vanilla_CALL,
    VK_Vanilla_BR6,
    VK_Vanilla_BR11
  };

private:
  const VariantKind Kind;
  const MCExpr *Expr;

  explicit VanillaMCExpr(VariantKind Kind, const MCExpr *Expr)
      : Kind(Kind), Expr(Expr) {}

public:
  /// @name Construction
  /// @{

  static const VanillaMCExpr *create(VariantKind Kind, const MCExpr *Expr,
                                 MCContext &Ctx);
  /// @}
  /// @name Accessors
  /// @{

  /// getOpcode - Get the kind of this expression.
  VariantKind getKind() const { return Kind; }

  /// getSubExpr - Get the child of this expression.
  const MCExpr *getSubExpr() const { return Expr; }

  /// getFixupKind - Get the fixup kind of this expression.
  Vanilla::Fixups getFixupKind() const { return getFixupKind(Kind); }

  /// @}
  void printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const override;
  bool evaluateAsRelocatableImpl(MCValue &Res,
                                 const MCAsmLayout *Layout,
                                 const MCFixup *Fixup) const override;
  void visitUsedExpr(MCStreamer &Streamer) const override;
  MCSection *findAssociatedSection() const override {
    return getSubExpr()->findAssociatedSection();
  }
  
  void fixELFSymbolsInTLSFixups(MCAssembler &Asm) const override;

  static bool classof(const MCExpr *E) {
    return E->getKind() == MCExpr::Target;
  }

  static bool classof(const VanillaMCExpr *) { return true; }

  static VariantKind parseVariantKind(StringRef name);
  static bool printVariantKind(raw_ostream &OS, VariantKind Kind);
  static Vanilla::Fixups getFixupKind(VariantKind Kind);
};

} // end namespace llvm.

#endif

