ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs) -lEG -lGui

FEDRALIBS    := -L$(FEDRAROOT)/lib -lvt -lEmath -lEphys -lEdb -lEbase -lEdr -lEIO -lAlignment -lGeom -lEdd -lEMC -lDataConversion -lScan -lEMC

TARGET1=$(FSEM)/app/bin/fl_read
TARGET2=$(FSEM)/app/bin/fl_ascii
TARGET3=$(FSEM)/app/bin/fl_dump
TARGET4=$(FSEM)/app/bin/fl_brick


CFLAGS=-O3 -D_FILE_OFFSET_BITS=64 -g 
CFLAGS+=-I include -Wall $(ROOTCFLAGS)
CFLAGS+=-I$(ROOTSYS)/include 

FFLAGS=-I$(FEDRAROOT)/include $(CFLAGS)

#-g -g0 -fPIC -pthread -DDEBUG -ggdb
LIBS=$(ROOTLIBS)
FLIBS=$(LIBS)
FLIBS+=$(FEDRALIBS)
CC=g++

%.o: src/%.cxx
	$(CC) $(CFLAGS) -o $@ -c $<

all: fl_brick fl_ascii fl_dump

flreader.o: FlData.o
fl_ascii.o: FlData.o
fl_brick.o: FlData.o src/fl_brick.cxx
	$(CC) $(FFLAGS) -o $@ -c src/fl_brick.cxx

fl_read: FlData.o flreader.o
	$(CC) $(CFLAGS) -o $(TARGET1) FlData.o flreader.o $(LIBS)
fl_ascii: FlData.o fl_ascii.o
	$(CC) $(CFLAGS) -o $(TARGET2) FlData.o fl_ascii.o $(LIBS)
fl_dump: FlData.o fl_dump.o
	$(CC) $(CFLAGS) -o $(TARGET3) FlData.o fl_dump.o $(LIBS)
fl_brick: FlData.o fl_brick.o
	$(CC) $(FFLAGS) -o $(TARGET4) FlData.o fl_brick.o $(FLIBS)
clean:
# 	$(MAKE) -C lib clean
	rm -f ./*.o $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) core
	
