TARGET = candykids
OBJS = candykids.o bbuff.o stats.o
CFLAGS = -Wall -g -std=c99 -Werror -D_POSIX_C_SOURCE=199309L -pthread -lrt
CC_C = gcc

all: clean $(TARGET)

%.o : %.c
	$(CC) -c $(CFLAGS) $<

$(TARGET) : $(OBJS)
	$(CC_C) $(CFLAGS) $(OBJS) -o $@

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
