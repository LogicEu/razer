# razer makefile

STD=-std=c99
WFLAGS=-Wall -Wextra
OPT=-O2
LIBS=fract photon imgtool mass utopia
CC=gcc
NAME=razer
SRC=*.c

OS=$(shell uname -s)
ifeq ($(OS),Darwin)
	OSFLAGS=-mmacos-version-min=10.9
else 
	OSFLAGS=-lm #-lpthread -D_POSIX_C_SOURCE=199309L
    WRFLAG=-Wl,--whole-archive
    WLFLAG=-Wl,--no-whole-archive
endif

LDIR=lib
IDIR=$(patsubst %,-I%/,$(LIBS))
LSTATIC=$(patsubst %,lib%.a,$(LIBS))
LPATHS=$(patsubst %,$(LDIR)/%,$(LSTATIC))
LFLAGS=$(patsubst %,-L%,$(LDIR))
LFLAGS += $(WRFLAG)
LFLAGS += $(patsubst %,-l%,$(LIBS))
LFLAGS += $(WLFLAG)
LFLAGS +=-lz -lpng -ljpeg

CFLAGS=$(STD) $(WFLAGS) $(OPT) $(IDIR)

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
