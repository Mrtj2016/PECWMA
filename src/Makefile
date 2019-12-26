# This works for both binary installed and source installed
# It seems that g++ has some problem, so clang++ is used.
# Note that I add -std=c++11 option here to support smart pointer
targetpath = $(shell pwd)/build/
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "mutator" && $$9 != "common"&& $$9 != "build") print $$9}')

CXX := clang++
CXXFLAGS := -fno-rtti -O0 -g -std=c++11  -w

# Your own LLVM build path
LLVM_SRC_PATH := /home/tang/tools/llvm
LLVM_BUILD_PATH := /home/tang/tools/build_release
LLVM_BIN_PATH 	:= $(LLVM_BUILD_PATH)/bin

# LLVM C++ options and LLVM link options
LLVM_CXXFLAGS := `${LLVM_BIN_PATH}/llvm-config --cxxflags`
LLVMLDFLAGS := `${LLVM_BIN_PATH}/llvm-config --ldflags --libs --system-libs`

CLANG_INCLUDES := \
	-I $(LLVM_SRC_PATH)/tools/clang/include \
	-I $(LLVM_BUILD_PATH)/tools/clang/include

EXES = pecwma
LIBSO = cpms.so
CUR_OBJS = $(wildcard *.cpp)
CLANGLIBS = \
				-lclangTooling\
				-lclangFrontendTool\
				-lclangFrontend\
				-lclangDriver\
				-lclangSerialization\
				-lclangCodeGen\
				-lclangParse\
				-lclangSema\
				-lclangStaticAnalyzerFrontend\
				-lclangStaticAnalyzerCheckers\
				-lclangStaticAnalyzerCore\
				-lclangAnalysis\
				-lclangARCMigrate\
				-lclangRewriteFrontend\
				-lclangRewrite\
				-lclangEdit\
				-lclangAST\
				-lclangLex\
				-lclangBasic\
				-lcurses\
				$(shell ${LLVM_BIN_PATH}/llvm-config --libs)\
				$(shell ${LLVM_BIN_PATH}/llvm-config --system-libs)


export makepath targetpath SUBDIRS CXX CXXFLAGS LLVM_SRC_PATH LLVM_BUILD_PATH  LLVM_BIN_PATH  LLVM_CXXFLAGS LLVMLDFLAGS CLANG_INCLUDES

all: $(SUBDIRS) $(EXES) clean

toso: $(LIBSO)


$(SUBDIRS):ECHO
	make -C $@

ECHO:
	@echo $(SUBDIRS)
	@echo begin compile
	@echo ${targetpath}

#$(CUR_OBJS): %.o:%.cpp
#	$(CXX) -c $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ -o ${targetpath}/$@


objs = $(wildcard ${targetpath}/*.o)

$(EXES):
	$(CXX) $(objs)  $(CLANGLIBS) $(LLVMLDFLAGS) -o ${targetpath}/../$@

$(LIBSO): $(CUR_OBJS)
	$(CXX) $^ $(CLANGLIBS) $(LLVMLDFLAGS) -fPIC -shared -o $@

.PHONY: all clean

#make clean manually
clean:
	rm ${targetpath}/*.o
	mv ${EXES} ./mutator/
