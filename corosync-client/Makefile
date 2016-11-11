## Compiler
CC = gcc
## Includes
INC = -Iinc -Iinc/corosync -Iinc/libqb -Iinc/libqb/qb -Iinc/corosync/totem -Iinc/libssh -Iinc/minunit
## Libraries
STATLIBS = lib/libcmap.a lib/libcorosync_common.a lib/libqb.a lib/libcfg.a /usr/lib/x86_64-linux-gnu/libssh.so
GLIBS = -lpthread -ldl
## Directories
IDIR = inc
SDIR = src/main
TDIR = src/tests
BDIR = bin
ODIR = obj
## Executables
OUT = $(BDIR)/corosync-client

_OBJS = client.o cluster_manager.o quorum_manager.o ssh_manager.o sftp_manager.o conf_manager.o client_cmapctl.o cs_errors.o client_errors.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: $(SDIR)/%.c build
	$(CC) $(INC) -c -o $@ $<

$(OUT): $(OBJS)
	$(CC) $(INC) -o $(OUT) $^ $(STATLIBS) $(GLIBS)
	
build:
	@mkdir -p bin
	@mkdir -p obj

## builds and runs tests, must be called after make 
tests:
	$(MAKE) -C $(TDIR)
	
testrun:
	bin/./client-testing

.PHONY: clean

	
clean:
	rm -rf bin obj
	find . -name "*~" -exec rm {} \;
	find . -name "*.o" -exec rm {} \;