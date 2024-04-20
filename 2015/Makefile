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

clean:
	@$(RM) /Q bin
.PHONY: clean