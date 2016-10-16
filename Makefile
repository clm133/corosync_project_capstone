## Compiler
CC = gcc
## Includes
## Directories
ODIR = obj
SDIR = src
BDIR = bin
## Executables
OUT = $(BDIR)/client

_OBJS = client.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: $(SDIR)/%.c build
	$(CC) -c -o $@ $< 

$(OUT): $(OBJS)
	$(CC) -o $(OUT) $^
	
build:
	@mkdir -p bin
	@mkdir -p obj

.PHONY: clean

clean:
	rm -rf bin obj
	find . -name "*~" -exec rm {} \;
	find . -name "*.o" -exec rm {} \;