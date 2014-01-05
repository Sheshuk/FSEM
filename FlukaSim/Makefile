TARGET=$(FSEM)/app/bin/FlukaSim

FFF=$(FLUPRO)/flutil/fff
LNK=$(FLUPRO)/flutil/lfluka
FFLAGS= 
FILES=$(shell ls $(FSEM)/$(FSim)/src/*.f)
OBJ=$(FILES:.f=.o)

%.o: %.f
	$(echo "Compiling object file \"$@\"")
	$(echo "====================================")
	$(FFF) $(FFLAGS) $<

all:    $(OBJ)
# 	echo "~~~~F $(FILES)"
# 	echo "~~~~O $(OBJ)"
# 	@for fle in $(FILES); do ((echo "FILE="$$fle)&& ($(FFF) $(FFLAGS) $$fle)); done;
	
	$(LNK) -o $(TARGET) -m fluka $(shell ls $(FSEM)/src/*.o)
	strip -s $(TARGET)

clean:
# 	$(MAKE) -C lib clean
	rm -f $(OBJ) $(TARGET) core
	rm -f $(TARGET)
	rm -f $(TARGET).map
