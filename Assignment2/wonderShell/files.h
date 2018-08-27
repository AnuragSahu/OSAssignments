#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pwd.h>
#include<signal.h>
#include"splitCom.c"
#include<sys/wait.h>
#include<limits.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

void split(char args[PATH_MAX]);
char* read_command();

