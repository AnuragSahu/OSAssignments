#include<stdio.h>
#include<stdlib.h>
#include<pwd.h>
#include<limits.h>
#include<string.h>
//#define _POSIX_SOURCE
#include <unistd.h>
#include"files.h"

char deviceName[100],currentWD[PATH_MAX];
char* userName;
char args[PATH_MAX];

char *getUserName()
{
  uid_t uid = geteuid();
  struct passwd *pw = getpwuid(uid);
  if (pw)
  {
    return pw->pw_name;
  }
  return "";
}

int main()
{
	gethostname(deviceName,sizeof(deviceName));
	userName = getUserName();char* args;
  while(1)
  {
    getcwd(currentWD,sizeof(currentWD));
	printf("<"MAG"%s"RESET"@"YEL"%s"RESET":"GRN"~%s"RESET">\n",userName,deviceName,currentWD);
//		printf("<%s@%s:~%s>",userName,deviceName,currentWD);
    args = read_command();
    splitcommands(args);
  }

}