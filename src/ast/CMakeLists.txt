
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(ast)

# Your own LLVM build path
SET(LLVM_SRC_PATH  /home/tang/tools/llvm)
SET(LLVM_BUILD_PATH  /home/tang/tools/build_release)
SET(LLVM_BIN_PATH  ${LLVM_BUILD_PATH}/bin)

SET(LLVM_CXXFLAGS  `${LLVM_BIN_PATH}/llvm-config --cxxflags`)
SET(LLVMLDFLAGS  `${LLVM_BIN_PATH}/llvm-config --ldflags --libs --system-libs`)

INCLUDE_DIRECTORIES(
$(LLVM_SRC_PATH)/tools/clang/include
$(LLVM_BUILD_PATH)/tools/clang/include
)

set(CMAKE_CXX_COMPILER "/usr/local/bin/clang++" ) # 显示指定使用的C++编译器
set(CMAKE_CXX_FLAGS "-std=c++11") # c++11
set(CMAKE_CXX_FLAGS "-g") # 调试信息
set(CMAKE_CXX_FLAGS "-w") # 开启所有警告
set(CMAKE_CXX_FLAGS "-O0" ) # 调试包不优化
set(CMAKE_CXX_FLAGS "-fno-rtti" )
#set(CMAKE_CXX_FLAGS "${LLVM_BIN_PATH}/llvm-config --cxxflags" )
#set(CMAKE_CXX_FLAGS "${LLVM_BIN_PATH}/llvm-config --ldflags --libs --system-libs" )

aux_source_directory(. DIR_SRCS)

# Add a library with the above sources
add_library(${PROJECT_NAME} ${DIR_SRCS})
add_library(sub::ast ALIAS ${PROJECT_NAME})
