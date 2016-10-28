## Compiler
CC = gcc
## Includes
INC = -Iinc -Iinc/corosync -Iinc/libqb -Iinc/libqb/qb -Iinc/corosync/totem -Iinc/libssh
## Libraries
STATLIBS = lib/libcmap.a lib/libcorosync_common.a lib/libqb.a lib/libcfg.a /usr/lib/x86_64-linux-gnu/libssh.so
SHAREDLIBS = -L/usr/lib/x86_64-linux-gnu/
GLIBS = -lpthread -ldl
## Directories
IDIR = inc
SDIR = src
BDIR = bin
ODIR = obj
## Executables
## Executables
OUT = $(BDIR)/corosync-client

_OBJS = client.o cluster_manager.o conf_writer.o modified_cmapctl.o client_errors.o ssh_manager.o
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