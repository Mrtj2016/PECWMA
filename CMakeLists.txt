CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(main)

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

SET(CMAKE_CXX_COMPILER "/usr/local/bin/clang++" ) # 显示指定使用的C++编译器
SET(CMAKE_CXX_FLAGS "-std=c++11") # c++11
SET(CMAKE_CXX_FLAGS "-g") # 调试信息
SET(CMAKE_CXX_FLAGS "-w") # 开启所有警告
SET(CMAKE_CXX_FLAGS "-O0" ) # 调试包不优化
SET(CMAKE_CXX_FLAGS "-fno-rtti" )

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

INSTALL()

# Add sub directories
add_subdirectory(src)