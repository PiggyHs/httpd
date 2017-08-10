#ifndef _HTTPD_
#define _HTTPD_
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<errno.h>
#include<strings.h>
#include<sys/stat.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<signal.h>
#include<sys/sendfile.h>

#define SIZE 1024
#define SERVER_STRING "Server:hshttpd.1.0.0\r\n"
int get_line(int sock,char*buf,int size);
int startup(const char* _ip,int _port);
void* handler_requst(void* arg);

static void execute_cgi(int client,const char *path,const char* method,char* query_string);
static void unimplemented(int client);
static void cat(int client,FILE* file);
static void not_found(int client);
static server_file(int client, const char *filename);
static void headers(int client,const char* filename);
#endif
