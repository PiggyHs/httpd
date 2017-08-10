#include "httpd.h"


int startup(const char* _ip,int _port)
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock <0){
		perror("socket");
		exit(2);
	}
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(_port);
	local.sin_addr.s_addr = inet_addr(_ip);

	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		perror("bind");
		exit(3);
	}
	if(listen(sock,10)<0)
	{
		perror("listen");
		exit(4);
	}
	return sock;
}

int get_line(int sock,char*buf,int size)
{
	int i = 0;
	char c = '\0';
	int n;

	while((i<size-1) && (c!='\n'))
	{
		n = recv(sock,&c,1,0);
		if(n>0)
		{
			if(c == '\r')
			{
				/*使用MSG_PEEK标志对下个元素进行窥探*/
				n = recv(sock,&c,1,MSG_PEEK);
				if((n>0)&&(c== '\n'))
					recv(sock,&c,1,0);
				else
					c = '\n';
			}
			buf[i] = c;
			i++;
		}
		else
			c = '\n';
	}
	buf[i] = '\0';
	return (i);
}

static void headers(int client,const char* filename)
{
	char buf[1024];
	(void)filename;

	/*http header*/
	strcpy(buf,"HTTP/1.0 200 OK\r\n");
	send(client,buf,strlen(buf),0);
	strcpy(buf, SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf,"Content-Type: text/html\r\n");
	send(client,buf,strlen(buf),0);
	strcpy(buf,"\r\n");
	send(client,buf,strlen(buf),0);

}

static void cat(int client,FILE* file)
{
	char buf[SIZE];

	fgets(buf,sizeof(buf),file);
	while(!feof(file))
	{
		send(client,buf,strlen(buf),0);
		fgets(buf,sizeof(buf),file);
	}
}

static server_file(int client, const char *filename)
{
	FILE * file = NULL;

	file = fopen(filename,"r");
	if(file == 0)
	{
		//not_found(client);
		printf("%s is not found..\n",filename);
	}
	else
	{
		cat(client,file);
	}
}

static void not_found(int client)
{
	char buf[1024];

	sprintf(buf,"HTTP/1.0 404 NOT FOUND\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,SERVER_STRING);
	send(client,buf,strlen(buf),0);
	sprintf(buf,"Content-Type : text/html\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "your request because the resource specified\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "is unavailable or nonexistent.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

static void unimplemented(int client)
{
	char buf[SIZE];

	sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
	send(client, buf, strlen(buf), 0);
	 /*服务器信息*/
	 sprintf(buf, SERVER_STRING);
	 send(client, buf, strlen(buf), 0);
     sprintf(buf, "Content-Type: text/html\r\n");
	 send(client, buf, strlen(buf), 0);
	 sprintf(buf, "\r\n");
	 send(client, buf, strlen(buf), 0);
     sprintf(buf, "<html><head><title>Method Not Implemented\r\n");
	 send(client, buf, strlen(buf), 0);
     sprintf(buf, "</title></head>\r\n");
	 send(client, buf, strlen(buf), 0);
	 sprintf(buf, "<body>HTTP request method not supported.\r\n");
	 send(client, buf, strlen(buf), 0);
	 sprintf(buf, "</body></html>\r\n");
	 send(client, buf, strlen(buf), 0);
}

void* handler_requst(void* arg)
{
	int sock = (int) arg;

#ifdef _DEBUG_
	int ret;
	char line[1024];
	do{
		int ret = get_line(sock,line,sizeof(line));
		if(ret>0){
			printf("%s",line);
		}else{
			printf("request.....done\n");
			break;
		}
	}while(1);
#endif

    int  ret = 0;
	char buf[SIZE];
	char method[SIZE/10];
	char url[SIZE];
	char path[512];
	int cgi = 0;
	int numchars;
	char * query_string = NULL;
	struct stat st;
	int i,j;
	if((numchars = get_line(sock,buf,sizeof(buf)))<=0){
		not_found(sock);
		goto end;
	}
	printf("get line success.. line length is %d\n",numchars);
	i = 0;
    j = 0;
	while(!isspace(buf[j])&&j<sizeof(buf)-1&&i<sizeof(method)-1)
	{
		method[i] = buf[j];
		i++;
		j++;
	}
	method[i]= '\0';
	printf("get a method:%s\n",method);
	//不区分大小写的判断 如果既不是GET也不是POST
	if(strcasecmp(method,"GET")&&strcasecmp(method,"POST"))
	{
		unimplemented(sock);//
		goto end;
	}
	/*POST的时候开启cgi*/
	if(strcasecmp(method,"POST") == 0)
		cgi = 1;
	/*读取url*/
	i= 0;
	while(isspace(buf[j]) && j < sizeof(buf)-1)
		j++;
	while(!isspace(buf[j])&&i<sizeof(url)-1)
	{
		url[i] = buf[j];
		i++;j++;
	}
	url[i] = '\0';
	printf("url is :%s\n",url);
	/*GET*/
	if(strcasecmp(method,"GET") == 0)
	{
		query_string = url;
		while((*query_string != '?') && (*query_string != '\0'))
			query_string++;

		if(*query_string == '?')
		{
			cgi = 1;
			*query_string = '\0';
			query_string++;
		}
	}
	printf("url is :%s\n",url);

	sprintf(path,"wwwroot%s",url);

    	printf("path:%s\n",path);
	if(path[strlen(path)-1] == '/')
		strcat(path,"index.html");
    	printf("path:%s\n",path);
	if(stat(path, &st) == -1)
	{
		while((numchars >0)&& strcmp("\n",buf))
			numchars = get_line(sock,buf,sizeof(buf));

		not_found(sock);
		goto end;
	}
	else
	{
		if((st.st_mode & S_IFMT) == S_IFDIR)
			strcat(path,"/index.html");
    	printf("path:%s\n",path);
		if((st.st_mode & S_IXUSR) ||(st.st_mode & S_IXGRP) ||\
				(st.st_mode & S_IXOTH))
			cgi = 1;
		if(!cgi)
			{
				headers(sock, path);
				server_file(sock,path);
			}
		else
		{
			execute_cgi(sock,path,method,query_string);//
		}
	}

end:
	close(sock);
	return (void*)ret;
}

static void execute_cgi(int client,const char *path,const char* method, char* query_string)
{
	char buf[1024];
	int cgi_output[2];
	int cgi_input[2];
	int i ;
	int pid;
	int numchars = 1;
	int content_length = -1;
	int status;

	buf[0]='A'; buf[1] = '\0';
	if(strcasecmp(method,"GET") == 0)
	{
		while((numchars >0) && (strcmp("\n",buf)))
			numchars = get_line(client,buf,sizeof(buf));
	}
	else
	{
		numchars = get_line(client,buf,sizeof(buf));
		while((numchars>0) && strcmp("\n",buf))
		{
			buf[15] = '\0';
			if(strcasecmp(buf,"Content-Length:") == 0)
				content_length = atoi(&buf[16]);
			printf("content_length:%d\n",content_length);
			numchars = get_line(client,buf,sizeof(buf));
		}

		if(content_length == -1)
		{
			//bad_requst(client);
			return;
		}
	}

	sprintf(buf,"HTTP/1.0 200 ok\r\n");
	send (client,buf,strlen(buf),0);
	const char *blank="Content-Type:text/html\r\n\r\n";
	send(client,blank,strlen(blank),0);

	if(pipe(cgi_output)==0 && pipe(cgi_input)==0){

    printf("guan dao success...\n");
	printf("cgi_input[0]:%d\n",cgi_input[0]);
	printf("cgi_input[1]:%d\n",cgi_input[1]);
	printf("cgi_output[0]:%d\n",cgi_output[0]);
	printf("cgi_output[1]:%d\n",cgi_output[1]);
	if((pid = fork()) < 0)
	{
		//cannot_execute(client);
		return;
	}

	if(pid > 0)
	{
		char c = 0;
		close(cgi_output[1]);//关闭sgi_output的写入端
		close(cgi_input[0]);//关闭sgi_input的读取端
		printf("I am father\n");
		printf("METHOD = %s\n",method);
		if(strcasecmp(method ,"POST") == 0)
		{
			for(i = 0;i<content_length;i++)
			{
				recv(client,&c,1,0);
			if(write(cgi_input[1],&c,1) != 1)
			{
				perror("write");
			}	
			}
		
		}
		while(read(cgi_output[0],&c,1)>0)
		{
			send(client,&c,1,0);
		}	
	waitpid(pid,NULL,0);

	close(cgi_output[0]);
	close(cgi_input[1]);
	}
	else{
		char meth_env[255];
		char query_env[255];
		char length_env[255];

		close(cgi_output[0]);//关闭cgi_output的读取端
		close(cgi_input[1]);//关闭cgi_input的写入端
		/*CGI规定子进程从0中读，1中写*/

		/*STDOUT 重定向到cgi_output 的写入端*/
		printf("Enter child ..\n");

		if(dup2(cgi_output[1],1)<0)
		{
			perror("dup2");
		}
		/* STDIN 重定向到cgi_input的读取端*/
		if(dup2(cgi_input[0],0)<0)
		{
			perror("dup2");
		}

		sprintf(meth_env,"METHOD=%s",method);
		putenv(meth_env);
		if(strcasecmp(method,"GET") == 0)
		{
			sprintf(query_env,"QUERY_STRING=%s",query_string);
			putenv(query_env);
		}
		else
		{
			sprintf(length_env,"CONTENT_LENGTH=%d",content_length);
			putenv(length_env);
		}

		execl(path,path,NULL);
	}
	}
}
