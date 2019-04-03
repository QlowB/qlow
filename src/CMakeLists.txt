cmake_minimum_required(VERSION 3.9)

set(PROJECT_NAME qlow)
project(${PROJECT_NAME})

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



find_package(BISON 3.0.0)
find_package(FLEX 2.4.0)
find_package(LLVM REQUIRED CONFIG)

if( NOT LLVM_FOUND )
  message(FATAL_ERROR "LLVM package can't be found. Set CMAKE_PREFIX_PATH variable to LLVM's installation prefix.")
endif()

if ( BISON_FOUND AND FLEX_FOUND )
    BISON_TARGET(QlowParser ast/syntax.y ${CMAKE_CURRENT_BINARY_DIR}/syntax.cpp)
    FLEX_TARGET(QlowLexer ast/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
    ADD_FLEX_BISON_DEPENDENCY(QlowLexer QlowParser)
else()
    FILE(GLOB AdditionalSources bison/*.cpp)
    include_directories(bison/)
endif()

FILE(GLOB CppSources *.cpp ast/*.cpp sem/*.cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(ast sem .)

add_executable(${PROJECT_NAME} ${BISON_QlowParser_OUTPUTS} ${FLEX_QlowLexer_OUTPUTS} ${CppSources} ${AdditionalSources})


include_directories( ${LLVM_INCLUDE_DIRS} )
link_directories( ${LLVM_LIBRARY_DIRS} )
add_definitions( ${LLVM_DEFINITIONS} )

if (${APPLE})
    # hack to include libc++fs installed with homebrew
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS "-L/usr/local/opt/llvm/lib")
endif()

if ( CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER MATCHES ".*clang" )
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall -Wextra -Wpedantic -pedantic -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable
    )
    if (${APPLE})
        target_link_libraries(${PROJECT_NAME} c++fs)
    else()
        target_link_libraries(${PROJECT_NAME} stdc++fs)
    endif()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUGGING")
endif()
if ( MSVC )
    target_compile_options(${PROJECT_NAME} PRIVATE "/W4")
endif()


if(ipo_supported)
    #set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()


#
# assuming no cross compiling, target is set to the platform we are compiling on
#

if (${APPLE})
    target_compile_definitions(${PROJECT_NAME} PRIVATE QLOW_TARGET_APPLE)
endif()

if (${WIN32})
    target_compile_definitions(${PROJECT_NAME} PRIVATE QLOW_TARGET_APPLE)
endif()

#explicit_llvm_config(${PROJECT_NAME} STATIC_LIBRARY)
llvm_config(${PROJECT_NAME})

llvm_map_components_to_libnames(llvm_libs X86 passes)

#message( ${llvm_libs} )
target_link_libraries(${PROJECT_NAME} LLVM) # ${llvm_libs})


#    MIRParser
#    LTO
#    Passes
#    ObjCARCOpts
#    LibDriver
#    MCJIT
#    XCoreDisassembler
#    XCoreCodeGen
#    XCoreDesc
#    XCoreInfo
#    XCoreAsmPrinter
#    X86Disassembler
#    X86AsmParser
#    SystemZDisassembler
#    SystemZCodeGen
#    SystemZAsmParser
#    SystemZDesc
#    SystemZInfo
#    SystemZAsmPrinter
#    SparcDisassembler
#    SparcCodeGen
#    SparcAsmParser
#    SparcDesc
#    SparcInfo
#    SparcAsmPrinter
#    PowerPCDisassembler
#    PowerPCCodeGen
#    PowerPCAsmParser
#    PowerPCDesc
#    PowerPCInfo
#    PowerPCAsmPrinter
#    NVPTXCodeGen
#    NVPTXDesc
#    NVPTXInfo
#    NVPTXAsmPrinter
#    MSP430CodeGen
#    MSP430Desc
#    MSP430Info
#    MSP430AsmPrinter
#    MipsDisassembler
#    MipsCodeGen
#    MipsAsmParser
#    MipsDesc
#    MipsInfo
#    MipsAsmPrinter
#    LanaiDisassembler
#    LanaiCodeGen
#    LanaiAsmParser
#    LanaiDesc
#    LanaiAsmPrinter
#    LanaiInfo
#    HexagonDisassembler
#    HexagonCodeGen
#    HexagonAsmParser
#    HexagonDesc
#    HexagonInfo
#    BPFDisassembler
#    BPFCodeGen
#    BPFAsmParser
#    BPFDesc
#    BPFInfo
#    BPFAsmPrinter
#    ARMDisassembler
#    ARMCodeGen
#    ARMAsmParser
#    ARMDesc
#    ARMInfo
#    ARMAsmPrinter
#    ARMUtils
#    AMDGPUDisassembler
#    AMDGPUCodeGen
#    AMDGPUAsmParser
#    AMDGPUDesc
#    AMDGPUInfo
#    AMDGPUAsmPrinter
#    AMDGPUUtils
#    AArch64Disassembler
#    AArch64CodeGen
#    AArch64AsmParser
#    AArch64Desc
#    AArch64Info
#    AArch64AsmPrinter
#    AArch64Utils
#    Interpreter
#    WindowsManifest
#    Symbolize
#    DebugInfoPDB
#    DebugInfoDWARF
#    LineEditor
#    X86CodeGen
#    GlobalISel
#    X86Desc
#    X86Info
#    MCDisassembler
#    X86AsmPrinter
#    X86Utils
#    SelectionDAG
#    AsmPrinter
#    CodeGen
#    FuzzMutate
#    DlltoolDriver
#    Option
#    Coroutines
#    ipo
#    Instrumentation
#    Vectorize
#    ScalarOpts
#    Linker
#    IRReader
#    AsmParser
#    InstCombine
#    BitWriter
#    AggressiveInstCombine
#    Coverage
#    ObjectYAML
#    OrcJIT
#    TransformUtils
#    ExecutionEngine
#    Target
#    Analysis
#    ProfileData
#    RuntimeDyld
#    Object
#    MCParser
#    BitReader
#    MC
#    DebugInfoCodeView
#    DebugInfoMSF
#    Core
#    BinaryFormat
#    TableGen
#    Support
#    Demangle
#)



#list(REMOVE_ITEM LIBRARY_LIST LLVM [])

#message(${LIBRARY_LIST})

#target_link_libraries(${PROJECT_NAME} ${LIBRARY_LIST})

