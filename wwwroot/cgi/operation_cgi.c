/*************************************************************************
	> File Name: operation_cgi.c
	> Author: hs
	> Mail:1497707753@qq.com 
	> Created Time: 2017年08月05日 星期六 09时42分40秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<strings.h>
void math(char* query_string)
{
	int buf[521];
	int i =0;
	while((*query_string) != '\0')
	{
		if(*query_string++  == '=')
		{
			buf[i++] = atoi(query_string);
		}
	}
	int num1 = buf[0];
	int num2 = buf[1];
	printf("%d + %d  = %d\n",num1,num2,num1+num2);
}

int main()
{
	char* query_string;
	char* method = getenv("METHOD");
	int i;
	int content_length;
	int count;
	char buff[512];
	if(strcasecmp(method,"GET") == 0)
	{
		query_string = getenv("QUERY_STRING");
		if(query_string == NULL)
		{
			printf("get query_string fail...\n");
		}
	}else
	{
		content_length = atoi(getenv("CONTENT_LENGTH"));
		printf("content_length:%d\r\n",content_length);

		
	  count=read(0 ,buff,content_length);
	  if(count < 0)
	  {
		  perror("read");
	}
	  buff[count] = '\0';
	  query_string = buff;
	}
	query_string = "NUmber1=100&number2=200";
	printf("query_string:%s\r\n",query_string);

	math(query_string);
	return 0;

}
