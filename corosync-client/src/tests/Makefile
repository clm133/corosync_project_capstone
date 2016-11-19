## Compiler
CC = gcc
## Directories
IDIR = ../../inc
SDIR = ../main
TDIR = ../tests
LDIR = ../../lib
ODIR = ../../obj
BDIR = ../../bin
## Includes
INC = -I$(IDIR) -I$(IDIR)/corosync -I$(IDIR)/libqb -I$(IDIR)/libqb/qb -I$(IDIR)/corosync/totem -I$(IDIR)/libssh -I$(IDIR)/minunit
## Libraries
STATLIBS = $(LDIR)/libcmap.a $(LDIR)/libcorosync_common.a $(LDIR)/libqb.a $(LDIR)/libcfg.a /usr/lib/x86_64-linux-gnu/libssh.so
GLIBS = -lpthread -ldl
## Executables
OUT = $(BDIR)/client-testing

_TOBJS = client_tests_main.o
TOBJS = $(patsubst %,$(ODIR)/%,$(_TOBJS))
OBJS = $(ODIR)/client_cmapctl.o $(ODIR)/ssh_manager.o $(ODIR)/sftp_manager.o $(ODIR)/client_errors.o 

$(ODIR)/%.o: $(TDIR)/%.c
	$(CC) $(INC) -c -o $@ $<

$(OUT): $(TOBJS)
	$(CC) $(INC) -o $(OUT) $^ $(OBJS) $(STATLIBS) $(GLIBS)

