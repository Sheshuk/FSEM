ROOTCFLAGS   = $(shell root-config --cflags)
RLIBS     = $(shell root-config --libs) -lEG -lGui

# small fix for NetBeans IDE. Switch it off for usual build
# FEDRAROOT    :=/data/sw/fedra/trunk
# FSEM :=~/soft/FSEM
#----------------------------------------------------------

FLIBS    = -L$(FEDRAROOT)/lib -lvt -lEmath -lEphys -lEdb -lEbase -lEdr -lEIO -lAlignment -lGeom -lEdd -lEMC -lDataConversion -lScan -lEMC

BINDIR=../app/bin
#$(FSEM)/app/bin
TARGET1=$(BINDIR)/fl_read
TARGET2=$(BINDIR)/fl_ascii
TARGET3=$(BINDIR)/fl_dump
TARGET4=$(BINDIR)/fl_brick


CFLAGS=-O3 -D_FILE_OFFSET_BITS=64 
CFLAGS+=-I include -Wall -fPIC -std=c++0x
LIBS=

RFLAGS=-I$(ROOTSYS)/include  $(ROOTCFLAGS)
FFLAGS=-I$(FEDRAROOT)/include

CFLAGS+=$(RFLAGS) $(FFLAGS)
#-g -g0 -fPIC -pthread -DDEBUG -ggdb
LIBS+=$(RLIBS) $(FLIBS)
CC=g++

%: %.o
	#compiling app: $@
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBS)
	
%.o: src/%.cxx
	$(CC) $(CFLAGS) -o $@ -c $^

%.a:
	ar rvs $@ $^

%.so:
	$(CC) -shared -o $@ $^

# rules:
all: fl_mcs fl_brick fl_simple fl_dump
#fl_ascii fl_dump fl_brick

libs: lib_fsem.a lib_fsem.so

lib_fsem.a: FlData.o

lib_fsem.so: FlData.o
# flreader.o: FlData.o
# fl_ascii.o: FlData.o

fl_mcs: FlData.o FlReaderApp.o
fl_simple: FlData.o FlReaderApp.o
fl_read: FlData.o flreader.o
fl_ascii: FlData.o fl_ascii.o
fl_dump: FlData.o FlReaderApp.o
fl_brick: FlData.o FlReaderApp.o GeomReader.o


GeomReader.o: src/GeomReader.cxx
	$(CC) $(CFLAGS) -o $@ -c $^ 
# fl_brick.o: FlData.o GeomReader.o 


# fl_brick: FlData.o fl_brick.o GeomReader.o

clean:
# 	$(MAKE) -C lib clean
	rm -f ./*.o $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) core lib_fsem.*
	
