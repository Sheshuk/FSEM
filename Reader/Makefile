ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs) -lEG -lGui

# small fix for NetBeans IDE. Switch it off for usual build
FEDRAROOT    :=/data/sw/fedra/trunk
FSEM :=~/soft/FSEM
#----------------------------------------------------------
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

%: %.o
	$(CC) $(CFLAGS) -o $(TARGET1) $^ $(LIBS)
	
%.o: src/%.cxx
	$(CC) $(CFLAGS) -o $@ -c $^

all: fl_ascii fl_dump fl_brick

# flreader.o: FlData.o`
# fl_ascii.o: FlData.o

fl_brick.o: FlData.o GeomReader.o

fl_read: FlData.o flreader.o
fl_ascii: FlData.o fl_ascii.o
fl_dump: FlData.o fl_dump.o
fl_brick: FlData.o fl_brick.o GeomReader.o

clean:
# 	$(MAKE) -C lib clean
	rm -f ./*.o $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) core
	
