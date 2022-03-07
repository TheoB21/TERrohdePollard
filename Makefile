SOURCES=$(wildcard *.c)
PROGS=$(SOURCES:.c=)
CFLAGS = -g
LDLIBS = -g -lm -lgmp


all: $(PROGS)

clean:
	rm -f $(PROGS)