ROOT=$(shell pwd)

CGI_SRC=$(shell ls *.c)
CGI=$(basename $(CGI_SRC))
CGI_OBJ=$(notdir $(CGI_SRC:.c=.o))

CC=gcc

$(CGI):$(CGI_OBJ)
	$(CC) -o $@ $<
%.o:%.c
	$(CC) -c $<
.PHONY:clean
clean:
	rm -f $(CGI)
	rm -f $(CGI_OBJ)
.PHONY:debug
debug:
	@echo $(CGI)
	@echo $(CGI_SRC)
	@echo $(CGI_OBJ)

