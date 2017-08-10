ROOT = $(shell pwd)
CC=gcc
BIN=httpd
SRC=httpd.c main.c#$(shell ls *.c)
OBJ=$(shell echo  $(SRC) | sed 's/\.c/\.o/g')
LDFLAGS=-lpthread

$(BIN):$(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
%.o:%.c
	$(CC) -c $<

.PHONY:clean
clean:
	rm -f $(BIN)
	rm -f *.o $(BIN)
.PHONY:debug
debug:
	echo $(SRC)
	echo $(OBJ)
