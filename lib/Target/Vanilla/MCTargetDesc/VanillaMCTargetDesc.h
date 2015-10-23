//===-- VanillaMCTargetDesc.h - Vanilla Target Descriptions -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Vanilla specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef VanillaMCTARGETDESC_H
#define VanillaMCTARGETDESC_H

#include "llvm/Support/DataTypes.h"
#include "llvm/Target/TargetRegisterInfo.h"

namespace llvm {
  class MCAsmBackend;
  class MCCodeEmitter;
  class MCContext;
  class MCInstrInfo;
  class MCObjectWriter;
  class MCRegisterInfo;
  class MCSubtargetInfo;
  class StringRef;
  class Target;
  class Triple;
  class raw_ostream;
  class raw_pwrite_stream;

  extern Target TheVanillaTarget;
  
  MCCodeEmitter *createVanillaMCCodeEmitter(const MCInstrInfo &MCII,
                                        const MCRegisterInfo &MRI,
                                        MCContext &Ctx);
  MCAsmBackend *createVanillaAsmBackend(const Target &T, const MCRegisterInfo &MRI,
                                    const Triple &TT, StringRef CPU);
  MCObjectWriter *createVanillaELFObjectWriter(raw_pwrite_stream &OS,
                                           uint8_t OSABI, bool IsLittleEndian);
} // End llvm namespace

// Defines symbolic names for Vanilla registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "VanillaGenRegisterInfo.inc"

// Defines symbolic names for the Vanilla instructions.
#define GET_INSTRINFO_ENUM
#include "VanillaGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "VanillaGenSubtargetInfo.inc"

#endif
