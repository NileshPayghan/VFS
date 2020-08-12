#include"header.h"

int strlenx(char* UserName)
{
if(UserName==NULL)
	return -1;
	int count=0;
	while(*UserName!='\0')
	{
	count++;
	UserName++;
	}
	
	return count;
}
