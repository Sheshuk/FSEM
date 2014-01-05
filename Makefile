FSim = "FlukaSim"
Tools = "Tools"
Reader = "Reader"

all: fsem reader

fsem: 
	$(MAKE) -C $(FSim)
reader:
	$(MAKE) -C $(Reader)
tools:
	$(MAKE) -C $(Tools)

clean: 
	rm app/bin/*
