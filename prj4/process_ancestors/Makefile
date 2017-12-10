# Define what compiler to use and the flags.
CC=gcc
CXX=g++
CCFLAGS= -g -std=c99 -D _GNU_SOURCE -static 

SRC = cs300_testapp_ancestors.c


# Compile all .c files into .o files
# % matches all (like * in a command)
# $< is the source file (.c file)
%.o : %.c
	$(CC) -c $(CCFLAGS) $<

array_test: $(SRC)
	$(CC) -o cs300_testapp_ancestors $(CCFLAGS) $(SRC)

transfer:
	scp -P 2222 cs300_testapp_ancestors root@localhost:~

clean:
	rm -f cs300_testapp_ancestors *.o
