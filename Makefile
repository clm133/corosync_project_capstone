IDIRQ=inc/inc_libqb
IDIRC=inc/inc_coro
LDIRQ=lib/lib_libqb
LDIRC=lib/lib_coro
LIBS=-L$(LDIRQ) -L$(LDIRC)
ODIR=obj
CC=gcc
CFLAGS=-I$(IDIRC) -I$(IDIRQ)
SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
EXECUTABLE=bin/client

all:    build $(EXECUTABLE)

$(EXECUTABLE):  $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(OBJECTS): src/%.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

build:
	@mkdir -p bin

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS) bin
	find . -name "*~" -exec rm {} \;
	find . -name "*.o" -exec rm {} \;