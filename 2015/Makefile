ifeq ($(OS), Windows_NT) 
RM = del
else
RM = rm
endif

CC = gcc
CFLAGS = -Wall -Wextra -O2
BIN = bin

%: %.c
	$(CC) $(CFLAGS) $^ -o $(BIN)/$@ && $(BIN)\$@
day_04: day_04.c md5.h md5.o
	$(CC) $(CFLAGS) day_04.c md5.o -o $(BIN)/day_04 && $(BIN)\day_04


md5.o: md5.c
	$(CC) $(CFLAGS) md5.c -c

clean:
	$(RM) *.exe *.o
.PHONY: clean