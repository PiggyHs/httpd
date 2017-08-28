#include "sqlApi.h"
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>


int main()
{

	char *query_string = NULL;
	char *method = getenv("METHOD");
	if(method){
		if( strcasecmp(method, "GET") == 0){
			query_string = getenv("QUERY_STRING");
		}else{
			char *len = getenv("CONTENT_LENGTH");
			int cl = atoi(len);
			int i = 0;
			char buf[1024];
			for(; i < cl; i++ ){
				read(0, buf+i, 1);
			}
			buf[i] = 0;
			query_string = buf;
		}
	}

	//name=''&sex=''&school=''&hobby=''
	char *start = query_string;
	char *myargv[5];
	int i = 0;
	while(*start){
		if(*start == '='){
			myargv[i++] = start + 1;
		}
		if(*start == '&'){
			*start = '\0';
		}
		start++;
	}
	myargv[i] = NULL;

	sqlApi sql;
	sql.connect();
	sql.insert(myargv[0], myargv[1], myargv[2], myargv[3]);
	//sql.select();
	return 0;
}


