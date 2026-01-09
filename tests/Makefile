CXXFLAGS=-Wall -Wpedantic -std=c++20 -I../utils -L../lib
LIBS=-lutils
EXECS=parse_tests
CXX=g++

all: $(EXECS)

$(EXECS): %: %.cpp
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

run:
	./parse_tests

clean:
	rm -rf $(EXECS)
