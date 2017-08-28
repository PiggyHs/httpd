#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

void mymath(char *data)
{
	//firstdata=123&lastdata=321
	char *start = data;
	char *myargv[3];
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
	myargv[i] = 0;
	int f = atoi(myargv[0]);
	int l = atoi(myargv[1]);
	printf("<html><body>");
	printf("<h1>%d + %d = %d</h1><br/>", f, l, f + l);
	printf("<h1>%d - %d = %d</h1><br/>", f, l, f - l);
	printf("<h1>%d * %d = %d</h1><br/>", f, l, f * l);
	printf("<h1>%d / %d = %d</h1><br/>", f, l, l == 0 ? -1 : f / l);
	printf("<h1>%d %% %d = %d</h1><br/>", f, l, f % l);
	printf("</body></html>");
	return;
}

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

	if(query_string){
		mymath(query_string);
	}
	return 0;
}







