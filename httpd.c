#include "httpd.h"

const char *status_line = "HTTP/1.0 200 OK\r\n";
const char *type_line = "Content-Type:text/html;charset=ISO-8859-1\r\n";
const char *blank_line = "\r\n";

int startup(const char *ip, int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("socket");
		exit(2);
	}

	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr(ip);

	if(bind(sock, (struct sockaddr*)&local, sizeof(local))< 0){
		perror("bind");
		exit(3);
	}

	if(listen(sock, BACKLOG) < 0){
		perror("listen");
		exit(4);
	}

	return sock;
}

void show_404(int sock)
{
	const char *error_line = "HTTP/1.0 404 Not Found\r\n";
	send(sock, error_line, strlen(error_line), 0);
	send(sock, type_line, strlen(type_line), 0);
	send(sock, blank_line, strlen(blank_line), 0);
	const char *error_html = "<html><h1>Not Found!</h1></html>";
	send(sock, error_html, strlen(error_html), 0);
}

void error_response(int sock, int err_code)
{
	switch(err_code){
		case 404:
			show_404(sock);
			break;
		case 400:
			break;
		case 401:
			break;
		case 403:
			break;
		case 500:
			break;
		case 503:
			break;
		default:
			break;
	}
}

int get_line(int sock, char* buff, int len)
{
	char c = '\0';
	int i = 0;
	while(i < len - 1 && c != '\n'){
		ssize_t s = recv(sock, &c, 1, 0);
		if(s > 0){
			if(c == '\r'){ //\ra \r\n \n
				recv(sock, &c, 1, MSG_PEEK);
				if(c == '\n'){
					recv(sock, &c, 1, 0);
				}else{
					c = '\n';
				}
			}
			buff[i++] = c;
		}else{
			break;
		}
	}
	buff[i] = 0;
	return i;
}

int echo_www(int sock, const char *path, int size)
{
	int fd = open(path, O_RDONLY);
	if(fd < 0){
		return 404;
	}
	send(sock, status_line, strlen(status_line), 0);
	send(sock, blank_line, strlen(blank_line), 0);
	sendfile(sock, fd, NULL, size);
	close(fd);
	return 200;
}

static void handler_header(int sock)
{
	char buf[SIZE];
	int ret = -1;
	do{
		ret = get_line(sock, buf, SIZE);
	}while(ret > 0 && strcmp(buf, "\n") != 0);
}

void exe_cgi(int sock, const char *method,\
		const char *query_string,\
		const char *path)
{
	char buf[SIZE];
	int content_len = -1;
	char method_env[SIZE];
	char query_string_env[SIZE];
	char content_len_env[SIZE];

	//get -> arg || POST
	if(strcasecmp(method, "GET") == 0){
		handler_header(sock);
	}else{//POST
		int ret = -1;
		do{
			ret = get_line(sock, buf, SIZE);
			if(ret > 0 && strncasecmp(buf,\
						"Content-Length: ", 16) == 0){
				content_len = atoi(&buf[16]);
			}
		}while(ret > 0 && strcmp(buf, "\n") != 0);
		if(content_len < 0){
			return;
		}
	}
	send(sock, status_line, strlen(status_line), 0);
	send(sock, type_line, strlen(type_line), 0);
	send(sock, blank_line, strlen(blank_line), 0);
	//get -> query_string, post -> content_len -> sock
	//path
	int input[2];
	int output[2];
	if(pipe(input) < 0){
		return;
	}
	if(pipe(output) < 0){
		return;
	}
	pid_t id = fork();
	if(id < 0){
		return;
	}else if(id == 0){//child
		close(input[1]);
		close(output[0]);
		dup2(input[0], 0);
		dup2(output[1], 1);

		sprintf(method_env, "METHOD=%s", method);
		putenv(method_env);
		if(strcasecmp(method, "GET") == 0){
			sprintf(query_string_env, "QUERY_STRING=%s", query_string);
			putenv(query_string_env);
		}else{
			sprintf(content_len_env, "CONTENT_LENGTH=%d", content_len);
			putenv(content_len_env);
		}
		execl(path, path, NULL);
		exit(1);
	}else{//father
		close(input[0]);
		close(output[1]);

		int i = 0;
		char c;
		if(strcasecmp(method, "POST") == 0){
			for( ; i < content_len; i++ ){
				recv(sock, &c, 1, 0);
				write(input[1], &c, 1);
			}

		}

		while(read(output[0], &c, 1) > 0){
			send(sock, &c, 1, 0);
		}

		waitpid(id, NULL, 0);
		close(input[1]);
		close(output[0]);
	}
}

void *handler_request(void *arg)
{
	int sock = (int)arg;
	char buff[SIZE];
	char method[SIZE/10];
	char url[SIZE];
	char path[SIZE];
	int err_code = 200;
	int i, j;
	int cgi = 0;
	char *query_string = NULL;
#ifdef _STDOUT_
	char buf[4096];
	ssize_t s = read(sock, buf, sizeof(buf)-1);
	buf[s] = 0;
	printf("%s", buf);
#else
	if(get_line(sock, buff, sizeof(buff)) <=0){
		err_code = 404;
		goto end;
	}
	printf("buff: %s\n", buff);
	//GET           /          HTTP/1.0
	i = 0;
	j = 0;
	while(i < sizeof(method)-1 && j < sizeof(buff) &&\
			!isspace(buff[j])){
		method[i] = buff[j];
		i++,j++;
	}
	method[i] = '\0';
	while(j < sizeof(buff) && isspace(buff[j])){
		j++;
	}
	i = 0;
	while(i < sizeof(url)-1 && j < sizeof(buff) && \
			!isspace(buff[j])){
		url[i] = buff[j];
		i++, j++;
	}
	url[i] = 0;

	if(strcasecmp(method, "GET") && strcasecmp(method, "POST")){
		err_code = 404;
		goto end;
	}

	if(strcasecmp(method, "POST") == 0){
		cgi = 1;
	}else{ //GET
		query_string = url;
		while(*query_string){
			if(*query_string == '?'){
				cgi = 1;
				*query_string = '\0';
				query_string++;
				break;
			}
			query_string++;
		}
	}

	//    /    /a/b/c
	sprintf(path, "wwwroot%s", url);
	if(path[strlen(path)-1] == '/'){
		strcat(path, "index.html");
	}

	struct stat st;
	if(stat(path, &st) < 0){
		err_code = 404;
		goto end;
	}else{
		if(S_ISDIR(st.st_mode)){
			strcat(path, "/index.html");
		}else if((st.st_mode & S_IXGRP) ||\
				 (st.st_mode & S_IXUSR) ||\
				 (st.st_mode & S_IXOTH) ){
			cgi = 1;
		}else{
		}
		//method, get(query_string), cgi, path
		if(cgi){
			exe_cgi(sock, method, query_string, path);
		}else{
			handler_header(sock);
			err_code = echo_www(sock, path, st.st_size);
		}
	}

#endif
end:
	if(err_code != 200){
		handler_header(sock);
		error_response(sock, err_code);
	}
	close(sock);
	return (void *)0;
}












