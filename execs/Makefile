CXXFLAGS=-Wall -Wpedantic -std=c++20 -I../utils -L../lib
LIBS=-lutils
EXECS=compile interpret
CXX=g++

all: $(EXECS)

$(EXECS): %: %.cpp
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	rm -rf $(EXECS)
