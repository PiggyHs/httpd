/*************************************************************************
	> File Name: main.c
	> Author: hs
	> Mail:1497707753@qq.com 
	> Created Time: 2017年08月02日 星期三 11时12分32秒
 ************************************************************************/

#include"httpd.h"
#include<pthread.h>
void Usage(const char* proc)
{
	printf("please see [local_ip][local_port]%s\n",proc);
}

int main(int argc,char* argv[])
{
	if(3!=argc)
	{
		Usage(argv[0]);
		return 1;
	}

	int listen_sock = startup(argv[1],atoi(argv[2]));
	printf("bind and listen success,wait accept...\n");

	while(1)
	{
		struct sockaddr_in client;
		socklen_t len =  sizeof(client);
		int new_sock = accept(listen_sock,(struct sockaddr*)&client,&len);
		if(new_sock < 0)
		{
			perror("accept");
			continue;
		}
		printf("get a new client: [%s:%d]\n",inet_ntoa(client.sin_addr),\
				ntohs(client.sin_port));
		
		pthread_t id;
		sigset_t signal_mask;
		sigemptyset (&signal_mask);
		sigaddset (&signal_mask, SIGPIPE);
		int rc = pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);
		if (rc != 0) {
			printf("block sigpipe error\n");
		}
		int ret = pthread_create(&id,NULL,handler_requst,(void*)new_sock);
		if(ret != 0)
		{
			perror("pthread_create");
			close(new_sock);
		}
		else
		{
			pthread_detach(id);
		}
	}
	close(listen_sock);
	return 0;
}
