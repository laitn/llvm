add_llvm_library(LLVMApprox
  Approx.cpp
  DESELECT.cpp
  OSR.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Transforms
  )

add_dependencies(LLVMApprox intrinsics_gen)
