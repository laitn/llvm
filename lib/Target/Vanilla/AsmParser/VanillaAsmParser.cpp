//===-- VanillaAsmParser.cpp - Parse Vanilla assembly to MCInst instructions --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/VanillaMCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCTargetAsmParser.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

namespace {
  class VanillaOperand;
  class VanillaAsmParser : public MCTargetAsmParser {

    MCSubtargetInfo &STI;
    MCAsmParser &Parser;

  /// @name Auto-generated Match Functions
  /// {

#define GET_ASSEMBLER_HEADER
#include "VanillaGenAsmMatcher.inc"

  /// }

  public:
    VanillaAsmParser(MCSubtargetInfo &sti, MCAsmParser &parser,
                const MCInstrInfo &MII,
                const MCTargetOptions &Options)
      : MCTargetAsmParser(Options), STI(sti), Parser(parser) {
      // Initialize the set of available features.
      //setAvailableFeatures(ComputeAvailableFeatures(STI.getFeatureBits()));
    }
    // public interface of the MCTargetAsmParser.
    bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                 OperandVector &Operands, MCStreamer &Out,
                                 uint64_t &ErrorInfo,
                                 bool MatchingInlineAsm) override;
    bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override;
    bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                          SMLoc NameLoc, OperandVector &Operands) override;
    bool ParseDirective(AsmToken DirectiveID) override;
    
    unsigned validateTargetOperandClass(MCParsedAsmOperand &Op,
                                        unsigned Kind) override;
  };
  /// SparcOperand - Instances of this class represent a parsed Sparc machine
  /// instruction.
  class VanillaOperand : public MCParsedAsmOperand {
  public:
    void addRegOperands(MCInst &Inst, unsigned N) const {
    }
    void addImmOperands(MCInst &Inst, unsigned N) const {
    }
    StringRef getToken() const {
      return StringRef();
    }
  };
} // end namespace

bool VanillaAsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                             OperandVector &Operands,
                                             MCStreamer &Out,
                                             uint64_t &ErrorInfo,
                                             bool MatchingInlineAsm) {
  return false;
}

bool VanillaAsmParser::
ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc)
{
  return false;
}

bool VanillaAsmParser::ParseInstruction(ParseInstructionInfo &Info,
                                      StringRef Name, SMLoc NameLoc,
                                      OperandVector &Operands) {
  return false;
}

bool VanillaAsmParser::
ParseDirective(AsmToken DirectiveID)
{
  return false;
}

extern "C" void LLVMInitializeVanillaAsmParser() {
  RegisterMCAsmParser<VanillaAsmParser> A(TheVanillaTarget);
}

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "VanillaGenAsmMatcher.inc"

unsigned VanillaAsmParser::validateTargetOperandClass(MCParsedAsmOperand &GOp,
                                                    unsigned Kind) {
  return 0;
}

