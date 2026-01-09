CXXFLAGS=-Wall -Wpedantic -std=c++20 -I../utils -L../lib
LIBS=-lutils
EXECS=run_tests
CXX=g++

all: $(EXECS)

$(EXECS): %: %.cpp
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

run:
	./run_tests -ni

clean:
	rm -rf $(EXECS)
