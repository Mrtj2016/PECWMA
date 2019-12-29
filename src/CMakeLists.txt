CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(pecwma)

# Add sub directories
add_subdirectory(ast)
add_subdirectory(ops)
add_subdirectory(rewriter)

aux_source_directory(. DIR_SRCS)
# Create the executable
add_executable(${PROJECT_NAME} ${DIR_SRCS})

target_link_libraries(${PROJECT_NAME}
PRIVATE
    sub::ast
    sub::ops
    sub::rewriter

    clangTooling
    clangFrontendTool
    clangFrontend
    clangDriver
    clangSerialization
    clangCodeGen
    clangParse
    clangSema
    clangStaticAnalyzerFrontend
    clangStaticAnalyzerCheckers
    clangStaticAnalyzerCore
    clangAnalysis
    clangARCMigrate
    clangRewriteFrontend
    clangRewrite
    clangEdit
    clangAST
    clangLex
    clangBasic
    curses
    LLVMLTO LLVMPasses LLVMObjCARCOpts LLVMSymbolize LLVMDebugInfoPDB LLVMDebugInfoDWARF LLVMMIRParser
    LLVMFuzzMutate LLVMCoverage LLVMTableGen LLVMDlltoolDriver LLVMOrcJIT
    LLVMXCoreDisassembler LLVMXCoreCodeGen LLVMXCoreDesc LLVMXCoreInfo LLVMXCoreAsmPrinter
    LLVMSystemZDisassembler LLVMSystemZCodeGen LLVMSystemZAsmParser LLVMSystemZDesc LLVMSystemZInfo
    LLVMSystemZAsmPrinter LLVMSparcDisassembler LLVMSparcCodeGen LLVMSparcAsmParser LLVMSparcDesc
    LLVMSparcInfo LLVMSparcAsmPrinter LLVMPowerPCDisassembler LLVMPowerPCCodeGen LLVMPowerPCAsmParser LLVMPowerPCDesc
    LLVMPowerPCInfo LLVMPowerPCAsmPrinter LLVMNVPTXCodeGen LLVMNVPTXDesc LLVMNVPTXInfo LLVMNVPTXAsmPrinter
    LLVMMSP430CodeGen LLVMMSP430Desc LLVMMSP430Info LLVMMSP430AsmPrinter LLVMMipsDisassembler LLVMMipsCodeGen
    LLVMMipsAsmParser LLVMMipsDesc LLVMMipsInfo LLVMMipsAsmPrinter LLVMLanaiDisassembler LLVMLanaiCodeGen
    LLVMLanaiAsmParser LLVMLanaiDesc LLVMLanaiAsmPrinter LLVMLanaiInfo LLVMHexagonDisassembler LLVMHexagonCodeGen
    LLVMHexagonAsmParser LLVMHexagonDesc LLVMHexagonInfo LLVMBPFDisassembler LLVMBPFCodeGen LLVMBPFAsmParser LLVMBPFDesc
    LLVMBPFInfo LLVMBPFAsmPrinter LLVMARMDisassembler LLVMARMCodeGen LLVMARMAsmParser LLVMARMDesc LLVMARMInfo LLVMARMAsmPrinter LLVMARMUtils
    LLVMAMDGPUDisassembler LLVMAMDGPUCodeGen LLVMAMDGPUAsmParser LLVMAMDGPUDesc LLVMAMDGPUInfo LLVMAMDGPUAsmPrinter LLVMAMDGPUUtils
    LLVMAArch64Disassembler LLVMAArch64CodeGen LLVMAArch64AsmParser LLVMAArch64Desc LLVMAArch64Info LLVMAArch64AsmPrinter LLVMAArch64Utils
    LLVMObjectYAML LLVMLibDriver LLVMOption  LLVMWindowsManifest LLVMX86Disassembler LLVMX86AsmParser LLVMX86CodeGen
    LLVMGlobalISel LLVMSelectionDAG LLVMAsmPrinter LLVMX86Desc LLVMMCDisassembler LLVMX86Info LLVMX86AsmPrinter LLVMX86Utils LLVMMCJIT LLVMLineEditor
    LLVMInterpreter LLVMExecutionEngine LLVMRuntimeDyld LLVMCodeGen LLVMTarget LLVMCoroutines LLVMipo LLVMInstrumentation LLVMVectorize LLVMScalarOpts
    LLVMLinker LLVMIRReader LLVMAsmParser LLVMInstCombine LLVMBitWriter LLVMAggressiveInstCombine LLVMTransformUtils LLVMAnalysis LLVMProfileData
    LLVMObject LLVMMCParser LLVMMC LLVMDebugInfoCodeView LLVMDebugInfoMSF LLVMBitReader LLVMCore LLVMBinaryFormat LLVMSupport LLVMDemangle
    rt dl pthread m
)
#gtest_main gtest LLVMTestingSupport
