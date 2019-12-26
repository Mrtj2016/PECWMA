heads = $(wildcard *.h)
SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))

all: $(OBJECTS)

$(OBJECTS): %.o:%.cpp 
	$(CXX) -c $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ -o ${targetpath}/$@
 
.PHONY: all clean

clean:
	rm -f $(OBJECTS)

