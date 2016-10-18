## Compiler
CC = gcc
## Includes
INC = -Iinc -Iinc/corosync -Iinc/libqb -Iinc/libqb/qb
## Libraries
STATLIBS = lib/libcmap.a lib/libcorosync_common.a lib/libqb.a
GLIBS = -lpthread -ldl
## Directories
IDIR = inc
SDIR = src
BDIR = bin
ODIR = obj
## Executables
## Executables
OUT = $(BDIR)/client

_OBJS = client.o client_cmap.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: $(SDIR)/%.c build
	$(CC) $(INC) -c -o $@ $<

$(OUT): $(OBJS)
	$(CC) $(INC) -o $(OUT) $^ $(STATLIBS) $(GLIBS)
	
build:
	@mkdir -p bin
	@mkdir -p obj

.PHONY: clean

clean:
	rm -rf bin obj
	find . -name "*~" -exec rm {} \;
	find . -name "*.o" -exec rm {} \;