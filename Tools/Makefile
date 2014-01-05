ROOTFLAGS    := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs) -lEG -lGui
FEDRALIBS    := -L$(FEDRAROOT)/lib -lvt -lEmath -lEphys -lEdb -lEbase -lEdr -lEIO -lAlignment -lGeom -lEdd -lEMC -lDataConversion -lScan -lEMC
TARGET1=$(FSEM)/app/bin/fb2fedra
TARGET2=$(FSEM)/app/bin/fb2opsim
TARGET3=$(FSEM)/app/bin/fb2simple
CFLAGS=-O3 -D_FILE_OFFSET_BITS=64 -Wall -g -g0 -DDEBUG -ggdb -fPIC -pthread

ROOTFLAGS+=-I$(ROOTSYS)/include 
FEDRAFLAGS=-I$(FEDRAROOT)/include $(ROOTFLAGS)
INCDIR= include
CC=g++


%.o: src/%.cxx
	$(CC) -c $(CFLAGS) $(FEDRAFLAGS) -I $(INCDIR) $< -o $@
        
all: fb2fedra

fb2fedra: FlReader.o fb2fedra.o
	
	$(CC) -o $(TARGET1) FlReader.o fb2fedra.o $(ROOTLIBS) $(FEDRALIBS)
	strip -s $(TARGET1)

fb2opsim: FlReader.o fb2opsim.o
	$(CC) -o $(TARGET2) FlReader.o fb2opsim.o $(ROOTLIBS) $(FEDRALIBS)
	strip $(TARGET2)

fb2simple: fb2simple.o	
	$(CC) -o $(TARGET3) fb2simple.o $(ROOTLIBS)
	strip -s $(TARGET3)

clean:
# 	$(MAKE) -C lib clean
	rm -f ./*.o $(TARGET1) $(TARGET2) $(TARGET3) core
	
