FSim = FlukaSim
TARGET=$(FSEM)/app/bin/FlukaSim
FFF=$(FLUPRO)/flutil/fff
LNK=$(FLUPRO)/flutil/lfluka
FFLAGS= 
FILES= src/binout.f src/gc2fc.f src/mdstck.f src/mgdraw.f src/source.f src/stupre.f \
src/stuprf.f src/usrein.f src/usreou.f src/usrini.f src/usrmed.f  src/usrout.f
OBJ=$(FILES:.f=.o)

%.o: %.f
	@echo "===================================="
	@echo "Compiling object file \"$@\""
	$(FFF) $(FFLAGS) $^
	@if [ -e $@ ]; \
	then \
	echo "=============DONE===================";  \
	else \
	echo "=============FAILED=================";  \
	fi;

all: $(OBJ)
	@echo "===================================="
	@echo "Compiling file \"$@\""
	@echo " --- $^"
# 	echo "~~~~F $(FILES)"
# 	@for fle in $(FILES); do ((echo "FILE="$$fle)&& ($(FFF) $(FFLAGS) $$fle)); done;
	$(LNK) -o $(TARGET) -m fluka $^
	strip -s $(TARGET)

clean:
# 	$(MAKE) -C lib clean
	rm -f $(OBJ) $(TARGET) core
	rm -f $(TARGET)
	rm -f $(TARGET).map
