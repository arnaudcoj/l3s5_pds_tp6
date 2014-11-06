CC      = gcc                              
CFLAGS  = -Wall -Werror -ansi -pedantic    
CFLAGS += -D_XOPEN_SOURCE=500              
CFLAGS += -g

all:do

do:
	$(CC) $(CFLAGS) -o do do.c

test:

clean:
	rm -rf *.o *~ do