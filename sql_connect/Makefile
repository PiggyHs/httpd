ROOT=$(shell pwd)
INCLUDE=-I $(ROOT)/lib/include
LIB=-L $(ROOT)/lib/lib

.PHONY:all
all:insert_cgi select_cgi

insert_cgi:insert_cgi.cpp sqlApi.cpp
	gcc -o $@ $^ $(INCLUDE) $(LIB) -lmysqlclient
select_cgi:select_cgi.cpp sqlApi.cpp
	gcc -o $@ $^ $(INCLUDE) $(LIB) -lmysqlclient

.PHONY:clean
clean:
	rm -f insert_cgi select_cgi

.PHONY:output
output:
	cp -f insert_cgi ../output/wwwroot/cgi-bin
	cp -f select_cgi ../output/wwwroot/cgi-bin
