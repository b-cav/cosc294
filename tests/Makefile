CXXFLAGS=-Wall -Wpedantic -std=c++20 -I../utils -L../lib
LIBS=-lutils
EXECS=test_bank
CXX=g++

all: $(EXECS)

$(EXECS): %: %.cpp
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

run:
	./test_bank -ni

clean:
	rm -rf $(EXECS)
