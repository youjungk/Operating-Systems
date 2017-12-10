TARGET = preassignment
OBJS = preassignment.o
CCFLAGS = -Wall -g -std=c99 -Werror -D_POSIX_C_SOURCE=199309L -pthread -lrt

CC_C = gcc

all: clean $(TARGET)

%.o : %.c
	$(CC) -c $(CCFLAGS) $<

$(TARGET) : $(OBJS)
	$(CC_C) $(CFLAGS) $(OBJS) -pthread -o $@
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
