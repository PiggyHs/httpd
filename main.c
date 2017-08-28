#include "httpd.h"

static void usage(const char *proc)
{
	printf("Usage:\n\t%s [local_ip] [local_port]\n\n", proc);
}

int main(int argc, char *argv[])
{
	if(argc != 3){
		usage(argv[0]);
		return 1;
	}

	int listen_sock = startup(argv[1], atoi(argv[2]));
	daemon(1, 0);
	while(1){
		struct sockaddr_in client;
		socklen_t len = sizeof(client);
		int rw_sock = accept(listen_sock,\
				(struct sockaddr*)&client,\
				&len);
		if(rw_sock < 0){
			perror("accept");
			continue;
		}
		printf("get a clinet: [%s:%d]\n",\
				inet_ntoa(client.sin_addr),\
				ntohs(client.sin_port));
		pthread_t id;
		pthread_create(&id, NULL, handler_request, (void *)rw_sock);
		pthread_detach(id);
	}
	return 0;
}











