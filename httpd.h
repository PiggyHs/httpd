#ifndef _HTTPD_H_
#define _HTTPD_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/sendfile.h>
#include <string.h>
#include <sys/stat.h>

#define BACKLOG 10
#define SIZE 1024

int echo_www(int sock, const char *path, int size);
int get_line(int sock, char *buff, int size);
int startup(const char *ip, int port);
void *handler_request(void *arg);
void exe_cgi(int sock, const char *method,const char *query_string,const char *path);
void error_response(int sock, int err_code);

#endif
