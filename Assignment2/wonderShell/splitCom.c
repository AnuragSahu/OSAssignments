// splitting the commands

#include<stdio.h>
#include<string.h>
#include<limits.h>

#define clear() printf("\033[H\033[J")

void executeInbuild()
{

}

void splitcommands(char str[PATH_MAX])
{
	char *dimlem = " | \n";
	char* anotherPointer;
	char* argumentList[50];
	char* token_r = strtok(str,dimlem,ptr1);
	if(strcmp("clear\n",token)==0)
	{
		clear();
		return;
	}
	while(token!=NULL)
	{
		printf("%s\n",token);
		token = strtok_r(NULL,dimlem,&ptr1);
	}
}

char* read_command()
{
  ssize_t bufferS = 0;
  char* command = NULL;
  if(getline(&command,&bufferS,stdin)==-1);
    //Cannot read the line

  return command;
}

void clearScr()
{
	clear();
}
