# MCTargetDesc/CMakeLists.txt
add_llvm_library(LLVMVanillaDesc
  VanillaMCTargetDesc.cpp
  VanillaAsmBackend.cpp
  VanillaMCCodeEmitter.cpp
  VanillaELFObjectWriter.cpp
  VanillaMCAsmInfo.cpp
  VanillaTargetStreamer.cpp
  VanillaMCExpr.cpp
  )
