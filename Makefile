CC      = gcc                              
CFLAGS  = -Wall -Werror -ansi -pedantic    
CFLAGS += -D_XOPEN_SOURCE=500              
CFLAGS += -g

all:do

do:do.c makeargv.o
	$(CC) $(CFLAGS) -o do makeargv.o do.c

makeargv.o:
	$(CC) -c $(CFLAGS) makeargv.c

test:

clean:
	rm -rf *.o *~ do