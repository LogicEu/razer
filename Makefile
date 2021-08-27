# tracy makefile

STD=-std=c99
WFLAGS=-Wall -Wextra
OPT=-O2
IDIR=-I. -Iinclude
LIBS=fract photon imgtool mass utopia
CC=gcc
NAME=razer
SRC=*.c

CFLAGS=$(STD) $(WFLAGS) $(OPT) $(IDIR)
OS=$(shell uname -s)

LDIR=lib
IDIR += $(patsubst %,-I%/,$(LIBS))
LSTATIC=$(patsubst %,lib%.a,$(LIBS))
LPATHS=$(patsubst %,$(LDIR)/%,$(LSTATIC))
LFLAGS=$(patsubst %,-L%,$(LDIR))
LFLAGS += $(patsubst %,-l%,$(LIBS))
LFLAGS += -lz -lpng -ljpeg

ifeq ($(OS),Darwin)
	OSFLAGS=-mmacos-version-min=10.9
else 
	OSFLAGS=-lm #-lpthread -D_POSIX_C_SOURCE=199309L
endif

$(NAME): $(LPATHS) $(SRC)
	$(CC) -o $@ $(SRC) $(CFLAGS) $(LFLAGS) $(OSFLAGS)

$(LDIR)/$(LDIR)%.a: $(LDIR)%.a $(LDIR)
	mv $< $(LDIR)/

$(LDIR): 
	mkdir $@

$(LDIR)%.a: %
	cd $^ && make && mv $@ ../

clean:
	rm -r $(LDIR) && rm $(NAME)
	
exe:
	$(CC) -o $(NAME) $(SRC) $(CFLAGS) $(LFLAGS) $(OSFLAGS)
