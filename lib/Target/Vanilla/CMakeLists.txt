set(LLVM_LINK_COMPONENTS
Approx
)

set(LLVM_TARGET_DEFINITIONS Vanilla.td)

tablegen(LLVM VanillaGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM VanillaGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM VanillaGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM VanillaGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM VanillaGenCallingConv.inc -gen-callingconv)

tablegen(LLVM VanillaGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM VanillaGenCodeEmitter.inc -gen-emitter)
tablegen(LLVM VanillaGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM VanillaGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM VanillaGenDisassemblerTables.inc -gen-disassembler)

add_public_tablegen_target(VanillaCommonTableGen)

add_llvm_target(VanillaCodeGen
  VanillaTargetMachine.cpp
  VanillaFrameLowering.cpp
  VanillaInstrInfo.cpp
  VanillaISelLowering.cpp
  VanillaRegisterInfo.cpp
  VanillaSubtarget.cpp
  VanillaISelDAGToDAG.cpp
  VanillaAsmPrinter.cpp
  VanillaMCInstLower.cpp
  VanillaTargetObjectFile.cpp
  VanillaVerifier.cpp
  VanillaOperatorSR.cpp
  )

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(InstPrinter)
add_subdirectory(AsmParser)
add_subdirectory(Disassembler)


