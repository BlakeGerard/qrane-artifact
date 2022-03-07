PROJ = aquma
LPROJ = aqumalib
TDRIV = test-driver

# Binary target
BIN = $(PROJ).exe

# Target library
TLIB = libaquma.so

#ISL = ./lib/isl
#ISL_LIBS = $(ISL)/lib
#ISL_INCL = $(ISL)/include

BARVINOK = ./lib/barvinok
BARVINOK_LIBS = $(BARVINOK)/lib
BARVINOK_INCL = $(BARVINOK)/include

NTL = ./lib/ntl
NTL_LIBS = $(NTL)/lib
NTL_INCL = $(NTL)/include

INCL = -I. -I$(BARVINOK_INCL) -I$(NTL_INCL)
LIBS = -L . -L $(BARVINOK_LIBS) -L$(NTL_LIBS) -lbarvinok -lisl -lpolylibgmp -lntl -lgmp -lm
BSRCS = $(PROJ).cc 
LSRCS = $(LPROJ).cc

IQM=itquma
IQMSRCS=$(IQM).cc
IQMBIN=$(IQM).exe

CXX = g++
CXXFLAGS = -Wall -O2 -g -std=c++11 -pthread 
CXXLIBFLAGS = -shared -fpic

#all: libaquma bin test
all: iter

bin: $(BSRCS) $(LSRCS) *.hh
	$(CXX) -o $(BIN) $(CXXFLAGS) $(INCL) $(BSRCS) $(LSRCS) $(LIBS) 

libaquma: $(LSRCS) *.hh
	$(CXX) -o $(TLIB) $(CXXFLAGS) $(CXXLIBFLAGS) $(INCL) $(LSRCS) $(LIBS) 

test: $(TDRIV).cc libaquma
	$(CXX) -o $(TDRIV).exe $(CXXFLAGS) $(INCL) $(TDRIV).cc $(LIBS) -laquma

iter: $(IQMSRCS) libaquma *.hh
	$(CXX) -o $(IQMBIN) $(CXXFLAGS) $(INCL) $(IQMSRCS) $(LIBS) -laquma

clean: 
	rm -f $(BIN)
