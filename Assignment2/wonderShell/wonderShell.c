#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pwd.h>
#include<limits.h>

//#define _POSIX_SOURCE
#include <unistd.h>
#include<sys/stat.h>
#include <sys/types.h>
#include<dirent.h>
#include<time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include<signal.h>



#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#define clear() printf("\033[H\033[J")

//#include"files.h"

char deviceName[100],currentWD[PATH_MAX];
char* userName;
char args[PATH_MAX];

int childpid;
char * childProcName;

time_t timSt;
double waitFor;
char msg[500];
//char * msg1;
int timeVLD = 0;
int loopflg=1;


void eleminate(int sig){
  loopflg = 0;
}
void clkCmd(char* list[50],int len)
{
	time_t timer;
	time_t timeSt = time(NULL);
	time_t timEd ;
    char buffer[26];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, 26, " %d %b %Y, %H:%M:%S", tm_info);
    
    if(len==1)
    {
    	puts(buffer);
    	return;
    }
    else if(len <3)
    {
    	printf("Wrong Usage, use -> clock -t <timelap>");
    	return;
    }
    else 
    {
    	puts(buffer);
    	char* ran;
    	int lap = strtol(list[2],&ran,10);
    	strcpy(buffer,"");
    	signal(SIGINT, eleminate); 
    	while(loopflg)
    	{
	    	timEd = time(NULL);
	    	if((timEd - timeSt)==lap)
			{
				tm_info = localtime(&timEd);
    			strftime(buffer, 26, " %d %b %Y, %H:%M:%S", tm_info);
				puts(buffer);
				timeSt = time(NULL);
			}    		
		}
		loopflg=1;
    }

}


char *replaceWord(const char *s, const char *oldW,const char *newW)
{
    char *result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(oldW);
 
    // Counting the number of times old word
    // occur in the string
    for (i = 0; s[i] != '\0'; i++)
    {
        if (strstr(&s[i], oldW) == &s[i])
        {
            cnt++;
 
            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }
 
    // Making new string of enough length
    result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);
 
    i = 0;
    while (*s)
    {
        // compare the substring with the result
        if (strstr(s, oldW) == s)
        {
            strcpy(&result[i], newW);
            i += newWlen;
            s += oldWlen;
        }
        else
            result[i++] = *s++;
    }
 
    result[i] = '\0';
    return result;
}

void replaceAll(char * str, char oldChar, char newChar)
{
  int i = 0;

  /* Run till end of string */
  while(str[i] != '\0')
  {
      /* If occurrence of character is found */
      if(str[i] == oldChar)
      {
          str[i] = newChar;
      }

      i++;
  }
}
void chkAlarm()
{
	if(timeVLD==0)
	{
	//	printf("No reminder\n");	
		return;
	}
	else if(timeVLD==1)
	{
		time_t timEd = time(NULL);
		double diff = difftime(timEd,timSt);
		if(diff>=waitFor)
		{
			timeVLD=0;
			printf("Reaminder:");
			printf("%s\n",msg);
			strcpy(msg,"");
		}
		else if(diff<0)
		{
			printf("Reaminder failed, printing message\n");
			printf("%s\n",msg);
			timeVLD=0;
			strcpy(msg,"");
		}
	}
	return;

}

void setAlarm(char* list[50],int len)
{
	if(len<3)
	{
		printf("Unable to set remaninder, Usage -> remindme <time in mins> \"message\"\n");
		return;
	}
	else if(timeVLD==1)
	{
		printf("Another timer is already set pls wait till it ends\n");
		return;
	}
	timeVLD=1;
	char *ptr;
	waitFor = strtol(list[1],&ptr,10);
	timSt = time(NULL);
	//setting the message
	int i;
	for(i=2;i<len;i++)
	{
		strcat(msg,list[i]);
		strcat(msg," ");
	}
	replaceAll(msg,'\"',' ');
	return;
}

extern void checkChild()
{
	// This function checks if the child process is still alive or not
	int st;
	int *status;
	pid_t a;
	status = &st;
	//FILE* statusf;
	//char path[70];
	//char processName[100];
	a= waitpid(-1, status, WNOHANG);
	if(a>0)
	{
		fprintf(stderr,"pid %d exicted Normally\n",a);
	}
}

int foreGroundProcess(char* line[50], int len)
{
	//Execution of foreground process
	line[len]=NULL;
	pid_t pid;
	//pid_t wpid;
  	int status;

	pid = fork();
	if (pid == 0)
	{
		// Child process
		if (execvp(line[0], line) == -1) 
		{
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	} 
	else if (pid < 0) 
	{
		// Error forking
		perror("lsh");
	} 
	else 
	{
		// Parent process
		do 
		{
			//wpid = waitpid(pid, &status, WUNTRACED);
			waitpid(pid, &status, WUNTRACED);
		} 
		while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

int backGroundProcess(char* line[50], int len)
{
	line[len]=NULL;
	pid_t pid;
	//pid_t wpid;
  	//int status;
  	//static int cpid;

	//  	printf("Creating Child %d\n",getpid());
	pid = fork();
	childpid=getpid();
	if (pid == 0)
	{
		// Child process
		childpid=getpid();
		printf("pid-- %d\n",childpid);
	//		printf("creating child\n");
		if (execvp(line[0], line) == -1) 
		{
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	} 
	else if (pid < 0) 
	{
		// Error forking
		perror("lsh");
	}
	else
	{
		//
	}
	return 1;
}

int switchForeOrBackGround(char* line[50], int len)
{
	if(strcmp(line[len-1],"&")==0)
	{
		//Back Ground Process call
		line[len-1]=NULL;
		len-=1;
		return backGroundProcess(line,len);
	}

	else
	{
		//ForeGround Process call
		childProcName = line[0];
		return foreGroundProcess(line,len);
	}
}



int chkFlgOrPath(char* list[50],int len)
{
	if(len==1)
		return 0;
	int ret=0;
	int i;
	for(i=1;i<len;i++)
	{
		if(strcmp(list[i],"-l")==0)
			ret+= 2;
		else if(strcmp(list[i],"-a")==0)
			ret+= 1;
		else if(strcmp(list[i],"-la")==0)
			return 3;
		else if(strcmp(list[i],"-al")==0)
			return 3;	
	}
	return ret;
}

int parsePath(char* list[50],int len)
{
	int i;
	for(i=1;i<len;i++)
	{
		if(list[i][0]!='-' && list[i][0]!='&')
		{
			return i;
		}
	}
	return -1;
}

void executeInbuild(char* list[50],int len)
{
	// Here I am going to implement the 
	// buid in methods defined in 
	// OS Assignement2.pdf
	checkChild();
	if(len==0)
	{
		return;
	}
	if(strcmp("clear",list[0])==0)
		clear();

	else if(strcmp("cd",list[0])==0)
	{
		if(len<1)
		{
			printf("Enter the path after cd ,Usgae -> cd <pathname>\n");
		}
		char* replceTilda = list[1];
		if(list[1][0]=='~'){
			replceTilda = replaceWord(list[1],"~",currentWD);
		//		printf("%s\n",replceTilda );
		}
		if(chdir(replceTilda)==-1)
			printf("No dir called : %s\n",list[1]);
	}

	else if(strcmp("pwd",list[0])==0)
	{
		char pwd[PATH_MAX];
		getcwd(pwd,sizeof(pwd));
		printf("%s\n",pwd);
	}

	else if(strcmp("remindme",list[0])==0)
	{
		setAlarm(list,len);
	}

	else if(strcmp("clock",list[0])==0)
	{
		clkCmd(list,len);
	}
	else if(strcmp("echo",list[0])==0)
	{
		int i=1;
		for(;i<len;i++)
			printf("%s ",list[i]);
		printf("\n");
		// Implement here the meathods echo
		//printf("echo yet not def\n");
	}

	else if(strcmp("ls",list[0])==0)
	{
		struct dirent *de;
		DIR* dr = NULL;
		int flg=0;
		for(int i=1;i<len;i++)
		{
			if(list[i][0]!='-' && list[i][0]!='&')
			{
				flg = 1;
				//char * path;
				dr = opendir(list[i]);
				break;
			}
		}
		if(flg==0) 	dr = opendir(".");

    char* file[5000];
    if (dr == NULL)
    {
        printf("Could not open file directory" );
        return;
    }
    int noFile=0;
    char* space = "  ";
    long int blksize =0;
    int infoLvl = chkFlgOrPath(list,len);
    while ((de = readdir(dr)) != NULL)
    {
    	file[noFile++] = de->d_name;
    	if(infoLvl==0){
				if(strcmp( ".",de->d_name)!=0 && strcmp("..",de->d_name)!=0)
					printf("%s%s", file[noFile-1],space);
			}
			else if(infoLvl==1)
				printf("%s%s", file[noFile-1],space);

			else if ((infoLvl==2 && strcmp( ".",de->d_name)!=0 && strcmp("..",de->d_name)!=0)||infoLvl==3)
			{
				struct stat fileStat;
				struct passwd *pwd;	    
				char date[12];
				int addrsI = parsePath(list,len);
				if(addrsI>0)
				{
					char path1[100];
					sprintf(path1,"%s%s",list[addrsI],file[noFile-1]);
					if(stat(path1,&fileStat)<0)
						return;
				}
				else{
					if(stat(file[noFile-1],&fileStat) < 0)    
		        return ;
				}

		    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
		    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
		    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
		    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
		    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
		    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
		    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
		    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
		    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
		    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
		    printf(" %ld ",fileStat.st_nlink);

	    	if ((pwd = getpwuid(fileStat.st_uid)) != NULL)
			    printf(" %-8.8s", pwd->pw_name);
				else printf(" %-8d", fileStat.st_uid);
	    	
	    	if ((pwd = getpwuid(fileStat.st_gid)) != NULL)
			    printf(" %-8.8s", pwd->pw_name);
				else printf(" %-8d", fileStat.st_gid);
	    	
	    	printf(" %8ld ",fileStat.st_size);
	    	strftime(date, 20, "%b %d %H:%M ", localtime(&(fileStat.st_mtime)));
	    	printf(" %s ",date);
	    	blksize += fileStat.st_blocks;
				printf("%s\n", file[noFile-1]);

			}
		}
		if(infoLvl>=2)
		{
	    blksize/=2;
	 		printf("total: %ld\n",blksize);
		 	blksize=0;
		 	printf("\n");
		}
		printf("\n");
		closedir(dr);
	}
	

	else if(len==1 && strcmp("pinfo",list[0])==0)
	{
		pid_t pid;
		pid= getpid();
		char path[50];
		sprintf(path,"/proc/%d/status",pid);
		FILE * filepointer = fopen(path,"r");
		if(filepointer==NULL)
		{
			printf("Unable to open status File, No Process of given pid %d\n",pid);
			return;
		}
		char procInfo[100];
		while(fgets(procInfo,sizeof(procInfo), (FILE*)filepointer)!=NULL)
		{
			if(strncmp("VmRSS:",procInfo,strlen("VmRSS"))==0)
				break;
		}
		printf("pid -- %d\n",pid);
		printf("%s","Process Status -- {R/S/S+/Z} memory\n\n - " );
   		char* token = strtok(procInfo, " \n");
        token = strtok(NULL, " \n");
        printf("%s {Virtual Memory}\n\n - ", token);
   		fclose(filepointer);
   		char exePath[200];
   		sprintf(path,"/proc/%d/exe",pid);
   		readlink(path,exePath,sizeof(exePath));
   		char currentWD[200];
   		getcwd(currentWD,sizeof(currentWD));
   		char* exePath1 = exePath1;
   		if(strncmp(currentWD,exePath,strlen(currentWD))==0)
   			exePath1 = replaceWord(exePath,currentWD,"~");
   		printf("%s\n\n",exePath1);
	}
	else if(len==2 && strcmp("pinfo",list[0])==0)
	{
		char pid[10];
		sprintf(pid,"%s",list[1]);
		char path[50];
		sprintf(path,"/proc/%s/status",pid);
		FILE * filepointer = fopen(path,"r");
		if(filepointer==NULL)
		{
			printf("No status File, no process with pid %s.\n",pid);
			return;
		}
		char procInfo[100];
		int flg=0;
		while(fgets(procInfo,sizeof(procInfo), (FILE*)filepointer)==NULL)
		{
			if(strncmp("VmRSS:",procInfo,strlen("VmRSS"))==0)
			{
				flg=1;
				break;
			}
		}
		if(flg==0)
		{
			printf("No Information Avilable\n");
			return;
		}
		printf("pid -- %s\n",pid);
		printf("%s","Process Status -- {R/S/S+/Z} memory\n\n - " );
   		char* token = strtok(procInfo, " \n");
        token = strtok(NULL, " \n");
        printf("%s {Virtual Memory}\n\n - ", token);
   		fclose(filepointer);
   		char exePath[200];
   		sprintf(path,"/proc/%s/exe",pid);
   		readlink(path,exePath,sizeof(exePath));
   		char currentWD[200];
   		getcwd(currentWD,sizeof(currentWD));
   		char* exePath1 = exePath1;
   		if(strncmp(currentWD,exePath,strlen(currentWD))==0)
   			exePath1 = replaceWord(exePath,currentWD,"~");
   		printf("%s\n\n",exePath1);
	}
	else switchForeOrBackGround(list,len);
}


void splitcommands(char str[PATH_MAX])
{
	char *dimlem = " \n";
	char *dimlem1 = ";";
	char* anotherPointer;
	char* anotherPointer1;
	int looper1=0;
	char* token;
	char* token1;
	token1 = strtok_r(str,dimlem1,&anotherPointer1);
	while(token1!=NULL)
	{
		char* argumentList[50];
		int looper=0;
		token = strtok_r(token1,dimlem,&anotherPointer);
		while(token!=NULL)
		{
			argumentList[looper++]=token;
			token = strtok_r(NULL,dimlem,&anotherPointer);
		}		
		token1 = strtok_r(NULL,dimlem1,&anotherPointer1);
		executeInbuild(argumentList,looper);

	}
}

char* read_command()
{
	ssize_t bufferS = 0;
	char* command = NULL;
	if(getline(&command,&bufferS,stdin)==-1){
		printf("Cannot Read pls enter again.\n");//Cannot read the line
		return read_command();
	}
	
	return command;
}

void clearScr()
{
	clear();
}

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
//	char currentWD[200];
	char shellsWD[200];
	getcwd(currentWD,sizeof(currentWD));
	gethostname(deviceName,sizeof(deviceName));
	userName = getUserName();
	char* args;
	while(1)
	{
		getcwd(shellsWD,sizeof(shellsWD));
		char *cwd1 = shellsWD;
		if(strncmp(currentWD,shellsWD,strlen(currentWD))==0)
				cwd1 = replaceWord(shellsWD,currentWD,"");

	printf("<"YEL"%s@%s"RESET":"BLU"~%s"RESET">\n",userName,deviceName,cwd1);
//		printf("<%s@%s:~%s>",userName,deviceName,currentWD);
	args = read_command();
	chkAlarm();
	if(strcmp("exit\n",args)==0)
	{
		printf("Exiting the Ultimate shell ever made the WonderShell\n");
		free(args);
		return 1;
	}
	splitcommands(args);
	free(args);
	}

}
