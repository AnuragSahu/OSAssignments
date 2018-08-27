#include<stdlib.h>
#include<stdio.h>

char* read_command()
{
	ssize_t buferS = 0;
	char* command = NULL;
	if(getline(&command,&bufferS,stdin)==-1)
		ctrlD();

	return command;
}