CC      = gcc                              
CFLAGS  = -Wall -Werror -ansi -pedantic    
CFLAGS += -D_XOPEN_SOURCE=500              
CFLAGS += -g

all:do

do:do.c makeargv.o
	$(CC) $(CFLAGS) -o do makeargv.o do.c

makeargv.o:makeargv.c
	$(CC) -c $(CFLAGS) makeargv.c

test:do
	./do "xclock" "xclock -d" "xclock -d -brief"

clean:
	rm -rf *.o *~ do