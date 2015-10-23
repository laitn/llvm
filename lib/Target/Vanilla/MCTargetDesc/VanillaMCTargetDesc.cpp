//===-- VanillaMCTargetDesc.cpp - Vanilla Target Descriptions -------------------===//
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

#include "VanillaMCTargetDesc.h"
#include "VanillaMCAsmInfo.h"
#include "VanillaMCAsmInfo.h"
#include "VanillaTargetStreamer.h"
#include "InstPrinter/VanillaInstPrinter.h"

#include "llvm/MC/MachineLocation.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "VanillaGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "VanillaGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "VanillaGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createVanillaMCInstrInfo() {
    MCInstrInfo *X = new MCInstrInfo();
    InitVanillaMCInstrInfo(X);
    return X;
}

static MCRegisterInfo *createVanillaMCRegisterInfo(const Triple &TT) {
    MCRegisterInfo *X = new MCRegisterInfo();
    InitVanillaMCRegisterInfo(X, Vanilla::R2);
    return X;
}

static MCSubtargetInfo *createVanillaMCSubtargetInfo(const Triple &TT,
                                                 StringRef CPU, StringRef FS) {
    return createVanillaMCSubtargetInfoImpl(TT, CPU, FS);
}

static MCCodeGenInfo *createVanillaMCCodeGenInfo(const Triple &TT, Reloc::Model RM,
                                                 CodeModel::Model CM,
                                                 CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();
  X->initMCCodeGenInfo(RM, CM, OL);
  return X;
}

static MCStreamer *createVanillaMCStreamer(const Triple &T,
                                           MCContext &Ctx, MCAsmBackend &MAB,
                                           raw_pwrite_stream &OS, MCCodeEmitter *Emitter,
                                           bool RelaxAll) {
  return createELFStreamer(Ctx, MAB, OS, Emitter, RelaxAll);
}

static MCInstPrinter *createVanillaMCInstPrinter(const Triple &T,
                                             unsigned SyntaxVariant,
                                             const MCAsmInfo &MAI,
                                             const MCInstrInfo &MII,
                                             const MCRegisterInfo &MRI) {
  return new VanillaInstPrinter(MAI, MII, MRI);
}

extern "C" void LLVMInitializeVanillaTargetMC() {
    // Register the MC asm info.
    RegisterMCAsmInfo<VanillaELFMCAsmInfo> X(TheVanillaTarget);
    // Register the MC codegen info.
    TargetRegistry::RegisterMCCodeGenInfo(TheVanillaTarget, createVanillaMCCodeGenInfo);
    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(TheVanillaTarget, createVanillaMCInstrInfo);
    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(TheVanillaTarget, createVanillaMCRegisterInfo);
    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(TheVanillaTarget,createVanillaMCSubtargetInfo);

    // Register the object streamer
    TargetRegistry::RegisterELFStreamer(TheVanillaTarget, createVanillaMCStreamer);
    // Register the MCInstPrinter.
    TargetRegistry::RegisterMCInstPrinter(TheVanillaTarget, createVanillaMCInstPrinter);
    // Register the MC code emitter
    TargetRegistry::RegisterMCCodeEmitter(TheVanillaTarget, createVanillaMCCodeEmitter);
    // Register the ASM Backend
    TargetRegistry::RegisterMCAsmBackend(TheVanillaTarget, createVanillaAsmBackend);
}
