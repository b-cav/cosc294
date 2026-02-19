CXXFLAGS=-Wall -Wpedantic -std=c++20
OFILES=helpers.o printer.o compiler.o interpreter.o
CXX=g++

all: $(OFILES)
	 ar cr ../lib/libutils.a $(OFILES)

%.o: %.c %.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o ../lib/libutils.a
