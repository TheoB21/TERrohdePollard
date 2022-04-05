SOURCES=$(wildcard *.c)
PROGS=$(SOURCES:.c=)
CFLAGS = -g
LDLIBS = -g -lm -lgmp -lpthread


all: $(PROGS)

mainTestSet : set.o
rhoPollard_Threads : set.o

.PHONY: clean
clean:
	rm -f $(PROGS)	
	rm -f *.o
