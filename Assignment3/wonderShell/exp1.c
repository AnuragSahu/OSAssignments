#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pwd.h>
#include<limits.h>
#include <unistd.h>
#include<time.h>
#include<dirent.h>
#include <fcntl.h>
#include<signal.h>
//#define _POSIX_SOURCE
#include<sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#define BUFFERSIZE_TOKEN 64
#define DILIMITOR_WORD " \t\n\r"
#define DILIMITOR_COMMAND ";\n"
#define DILIMITOR_PIPE "|\n"

#define clear() printf("\033[H\033[J")

struct command
{
	char **argv;
	int len;
};

typedef struct backGroundJobs
{
  char jobName[100];
  pid_t pid;
  char* status;
  struct backGroundJobs * next;
}backGroundJobs;
backGroundJobs* ThisJobs=NULL;

char deviceName[100],currentWD[PATH_MAX];
char* userName;
char args[PATH_MAX];

int childpid;
char * childProcName;

time_t timSt;
double waitFor;
char msg[500];
int timeVLD = 0;
int loopflg=1;
int foreground=0;
int chldrun=0;
int shell_pid,Childpid=0;

char * currentProcessName;

void HandlingPiping(char str[PATH_MAX]);

void executeInbuild(char* list[50],int len);

char *replaceWord(const char *s, const char *oldW,const char *newW)
{
    char *result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(oldW);
    for (i = 0; s[i] != '\0'; i++)
    {
        if (strstr(&s[i], oldW) == &s[i])
        {
            cnt++;
            i += oldWlen - 1;
        }
    }
    result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);
    i = 0;
    while (*s)
    {
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

int add_job(char *stat,pid_t pid, char * name)
{
	backGroundJobs *new=(backGroundJobs*)malloc(sizeof(backGroundJobs));
	new->pid=pid;
	new->status = stat;
	strcpy(new->jobName,name);
	new->next=NULL;
	if (ThisJobs==NULL)
	{
		ThisJobs=new;
	}
	else
	{
		backGroundJobs *temp=ThisJobs;

		for (;temp->next!=NULL;temp=temp->next);
		temp->next=new;
	}
}

int delete_job(pid_t pid)
{
	foreground=0;
	if (ThisJobs==NULL) {
		return 1;
	}
	if (ThisJobs->pid==pid) {
		backGroundJobs *del=ThisJobs;
		ThisJobs=ThisJobs->next;
		free(del);
		return 0;
	}
	backGroundJobs *temp=ThisJobs,*prev;
	for (prev=ThisJobs;temp->pid!=pid || temp==NULL;temp=temp->next)
	{
		if (temp!=ThisJobs) {
		prev=prev->next;
	}
	}
	//backGroundJobs *del=temp->next;
	if(temp!=NULL)
	{
		prev->next=temp->next;
		free(temp);
	}
}

int search_job(pid_t pid)
{

	if(ThisJobs ==NULL)
	{
		return 0;
	}
	//printf("Searching for %d\n",pid);
	if (ThisJobs==NULL) {
		return 0;
	}
	if (ThisJobs->pid==pid) {
		//printf(" found");
		return 1;
	}
	backGroundJobs *temp=ThisJobs;
	for (;temp->next!=NULL;temp=temp->next);
	{
		if(temp->pid==pid)
		{
			printf(" found");
			return 1;
		}
	}
	return 0;
}

int change_to_running(pid_t pid)
{

	if(ThisJobs ==NULL)
	{
		return 0;
	}
	if (ThisJobs->pid==pid) {
		ThisJobs->status = "Stopped";
		return 1;
	}
	backGroundJobs *temp=ThisJobs;
	for (;temp->next!=NULL;temp=temp->next);
	{
		if(temp->pid==pid)
		{
			ThisJobs->status = "Stopped";
			return 1;
		}
	}
	return 0;
}

int is_job_running(pid_t pid)
{

	if(ThisJobs ==NULL)
	{
		return 0;
	}
	if (strcmp(ThisJobs->status,"Running")==0) {
		//printf(" found");
		return 1;
	}
	backGroundJobs *temp=ThisJobs;
	for (;temp->next!=NULL;temp=temp->next);
	{
		if(strcmp(ThisJobs->status,"Running")==0)
		{
			return 1;
		}
	}
	return 0;
}

int get_job_pid(int number)
{
	backGroundJobs *temp=ThisJobs;
	
	if(ThisJobs ==NULL)
	{
		return 0;
	}

	if(number==1 && temp)
		return temp->pid;
		
	for(int i=0;i<number;i++)
	{
		if(temp==NULL)
			return -1;
		else
			temp=temp->next;
	}
	
	return temp->pid;
}

char* get_job_name(int number)
{
	backGroundJobs *temp=ThisJobs;
	
	if(number==1 && temp)
		return temp->jobName;
		
	for(int i=0;i<number;i++)
	{
		if(temp==NULL)
			return "";
		else
			temp=temp->next;
	}
	
	return temp->jobName;
}

void update_job_status()
{
	
	backGroundJobs *temp=ThisJobs;
	if(ThisJobs==NULL)
	{
		return;
	}
	else{
		int i=0;
		for(i=0;temp!=NULL;i++)
		{
			int pid = temp->pid;
			char path[50];
			int proc;
			char stat;
			sprintf(path,"/proc/%d/stat",pid);
			FILE * filepointer = fopen(path,"r");
			if(filepointer==NULL)
			{
				//printf("No status File, no process with pid %d.\n",pid);
				return ;
			}
			sprintf(path,"/proc/%d/stat",pid);
			fscanf(filepointer, "%d %*s %c",&proc,&stat);
			char* process_status;
			if(stat=='R')
			{
				process_status = "Running";
			}
			else if(stat=='S')
			{
				process_status = "Sleeping";
			}
			else if(stat=='Z')
			{
				process_status="Zombie";
			}
			temp->status = process_status;
			temp = temp->next;
		}
	}

}
void setEnvPressed(char *list[50], int len)
{
	if(len<2 && len > 3)
	{
		fprintf(stdout,"Wrong usage: -> setenv var [val]\n");
	}
	else if(len==2)
	{
		if(setenv(list[1],"",1)==0)
		{	fprintf(stdout,"Environment Variable Set : %s = %s\n",list[1],""); }
		else{
			fprintf(stdout,"Unable ot set enviroment variable\n");
		}

	}
	else
	{
		if(setenv(list[1],list[2],1)==0)
		{
			fprintf(stdout,"Environment Variable Set : %s = %s\n",list[1],list[2]);
		}else{
			fprintf(stdout,"Unable ot set enviroment variable\n");
		}
	}
}

void unSetEnvPressed(char* list[50],int len)
{
	if(len!=2)
	{
		fprintf(stdout,"Wrong Usage: -> unsetenv var\n");
	}
	else
	{
		if(unsetenv(list[1])==0)
		{
			fprintf(stdout,"Successfully Usetted the enviroment variable %s.\n",list[1]);
		}
	}
}

void fgPressed(char **list)
{
	if(list[1]==NULL)
		printf("Syntax Error!\n");
	else
	{
		char* ptr;
		pid_t pid=strtol(list[1],&ptr,10);
		//pid_t pid=make_int(list[1]),
		pid_t wpid;
		int status;
		do{
			wpid=waitpid(pid, &status,WUNTRACED);
		}while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}

void bgPressed(char* list[50],int len)
{
	/*	int currentProcess = getpid();
	if(currentProcess!=shell_pid)
	{

	}*/
	if(len !=2)
	{
		printf("Wrong Usage : bg <val>\n");
	}
	char *ptr;
	int jobnumber = strtol(list[1],&ptr,10);
	int chkpid;
	if((chkpid=get_job_pid(jobnumber))==-1)
	{
		printf("No such jobid\n");
		return;
	}
	if(search_job(chkpid)==1)
	{
		if(is_job_running(chkpid)==1)
		{
			fprintf(stdout,"Already Running\n");
			return;
		}
		else
		{
			
			change_to_running(chkpid);
			kill(chkpid,SIGTTIN);
			kill(chkpid,SIGCONT);
		}

	}
}
void kjobPressed(char **list)
{
	if(list[1]==NULL || list[2]==NULL)
	{
		printf("Invalid Syntax usage -> kjob <jobNumber> <signalNumber>");
		return;
	}
	char* ptr1, *ptr2;
	pid_t pid=strtol(list[1],&ptr1,10);
	int signal=strtol(list[2],&ptr2,10);
	kill(pid,signal);
}

void jobsPressed()
{
	//printf("helo\n");
	backGroundJobs *curr=ThisJobs;
	int i;
	for (i=1;curr!=NULL;curr=curr->next,i++) {
		printf("[%d] %s %s [%d]\n",i,curr->status,curr->jobName,curr->pid);
	}
	//return 1;
}

void overKillPressed()
{
	backGroundJobs *curr=ThisJobs;
	for(;curr!=NULL;curr=curr->next)
	{
		pid_t pid=curr->pid;
		kill(pid,SIGQUIT);
	}
}

void ctrlC()
{

	if(getpid()!=shell_pid)
		return;

	if(Childpid!=0)
	{
		fprintf(stdout,"\nSending SIGINT to %d.\n",Childpid);
		kill(Childpid,SIGINT);
		
	}
	signal(SIGINT, ctrlC);
	
}

void ctrlZ()
{
	if(getpid()!=shell_pid)
		return;
	printf("%d,%d\n",shell_pid,Childpid);

	if(Childpid!=shell_pid && Childpid!=0)
	{
		char * name = get_job_name(Childpid);
		add_job("Stopped",Childpid,name);
		kill(Childpid,SIGTTIN);
		kill(Childpid,SIGTSTP);
		
	}
	signal(SIGTSTP, ctrlZ);
	

}

void pinfoEntered(char* list[50],int len)
{
	char path[50], path1[50],path2[50];
	char exePath[100];
	char stat;
	int proc;
	char pid[10];
	if(len==2)
		sprintf(pid,"%s",list[1]);
	else{
		int a = getpid();
		sprintf(pid,"%d",a);
	}
	
	sprintf(path,"/proc/%s/stat",pid);
	sprintf(path1,"/proc/%s/statm",pid);
	sprintf(path2,"/proc/%s/exe",pid);
	FILE * filepointer = fopen(path,"r");
	if(filepointer==NULL)
	{
		printf("No status File, no process with pid %s.\n",pid);
		return;
	}
	fscanf(filepointer, "%d %*s %c",&proc,&stat);
	printf("Process Statues -- %c\n",stat);
	FILE * filepointer1 = fopen(path1,"r");
	if(filepointer==NULL)
	{
		printf("No statm File.\n");
		return;
	}
	long unsigned int mem;
	fscanf(filepointer1,"%lu",&mem);
	printf("pid -- %s\n",pid);
	printf("%lu, {Virtual memory}\n\n",mem);
	fclose(filepointer);
	readlink(path2,exePath,sizeof(exePath));
	printf(" -%s\n\n",exePath);


}

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


void replaceAll(char * str, char oldChar, char newChar)
{
  int i = 0;
  while(str[i] != '\0')
  {
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

void checkChild()
{
	// This function checks if the child process is still alive or not
	int st;
	int *status;
	pid_t a;
	status = &st;
	a= waitpid(-1, status, WNOHANG);
	if(a>0)
	{
		fprintf(stderr,"pid %d exicted Normally\n",a);
		delete_job(a);
		foreground=0;
	}
}

int foreGroundProcess(char* line[50], int len)
{
	//Execution of foreground process
	line[len]=NULL;
	currentProcessName = line[1];
	pid_t pid;
  	int status;
	pid = fork();
	Childpid=pid;
	if (pid == 0)
	{
		// Child process
		if (execvp(line[0], line) == -1) 
		{
			perror("lsh");
		}
		foreground =1;
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
	pid = fork();
	childpid=getpid();
	Childpid=pid;

	if (pid == 0)
	{
		// Child process
		childpid=getpid();
		printf("pid-- %d\n",childpid);
		if (execvp(line[0], line) == -1) 
		{
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	} 
	else if (pid < 0) 
	{
		// Error forking
		perror("Error Handled: lsh");
	}
	else
	{
		//add_job(pid,line[0]);
		// DO NO THING HERE!!
	}
	char path[50];
	int proc;
	char stat;
	sprintf(path,"/proc/%d/stat",pid);
	FILE * filepointer = fopen(path,"r");
	if(filepointer==NULL)
	{
		printf("No status File, no process with pid %d.\n",pid);
		return 0 ;
	}
	sprintf(path,"/proc/%d/stat",pid);
	fscanf(filepointer, "%d %*s %c",&proc,&stat);
	char* process_status;
	if(stat=='R')
	{
		process_status = "Running";
	}
	else if(stat=='S')
	{
		process_status = "Sleeping";
	}
	else if(stat=='Z')
	{
		process_status="Zombie";
	}
	add_job(process_status,pid,line[0]);
	return 1;
}

int switchForeOrBackGround(char* line[50], int len)
//int switchForeOrBackGround(char** args)
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

	else if(strcmp("fg",list[0])==0)
		fgPressed(list);

	else if(strcmp("bg",list[0])==0)
		bgPressed(list,len);


	else if(strcmp("kjob",list[0])==0)
		kjobPressed(list);

	else if(strcmp("jobs",list[0])==0)
		jobsPressed();

	else if(strcmp("overkill",list[0])==0)
		overKillPressed();

	else if(strcmp("setenv",list[0])==0)
		setEnvPressed(list,len);

	else if(strcmp("bg",list[0])==0)
		bgPressed(list,len);

	else if(strcmp("unsetenv",list[0])==0)
		unSetEnvPressed(list,len);

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
	

	else if(strcmp("pinfo",list[0])==0)
	{
		pinfoEntered(list,len);
		/*pid_t pid;
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
   		printf("%s\n\n",exePath1);*/
	}
	else switchForeOrBackGround(list,len);
	//switchForeOrBackGround(list);
}

void outputFileAt(char* list[50],int len, int n, int appendOrNot)
{
	int targetFilefd;
	if(appendOrNot==1)
		targetFilefd = open(list[n+1],O_WRONLY | O_RDONLY |O_APPEND | O_CREAT , 0644 );
	else targetFilefd = open(list[n+1],O_WRONLY | O_RDONLY | O_CREAT , 0644 );
	if(targetFilefd<0)
	{
		printf("Cannot open file %s.\n",list[n+1]);
		return;
	}
	if(dup2(targetFilefd,1)!=1)
	{
		printf("Error in shifting the filepointer to %s.\n",list[n+1]);
		return;
	}
	len-=2;
	executeInbuild(list,len);
	close(targetFilefd);
	freopen("/dev/tty","w",stdout);
	return;
}

void inputFileFrom(char* list[50],int len,int n)
{
	if(n==len-1)
	{
		printf("Missing Input file name;\nUsage-> python test_code.py < input.txt \n");
		return;
	}
	list[n] = NULL;
	char* targetFilename = list[n+1];
	int targetFilefd = open(targetFilename, O_RDONLY, 0644);
	if(targetFilefd == -1)
		perror("Failed to open file");
	close(0); 
	if(dup2(targetFilefd, 0) == -1) 
		perror("dup2 fail");
	executeInbuild(list,len);
	close(targetFilefd);
	
	printf("Here\n");
}
void chkFile(char* list [50],int len)
{
	int n=0;
	int inrediredted = 0;
	int redirected = 0;
	while(n<len)
	{
		if(strcmp(list[n],"<")==0)
		{
			inrediredted =1;
			redirected=1;
			printf("InputFile redirection %s.\n",list[n+1]);
			inputFileFrom(list,len,n);
		}
		else if(strcmp(list[n],">")==0)
		{
			redirected = 1;
			if(n==len-1)
			{
				printf("Output file is missing.\n Usage -> command > filename.txt\n");
				return;
			}
			printf("Redirected to file %s!!\n",list[n+1]);
			outputFileAt(list,len,n,0);
		}
		else if(strcmp(list[n],">>")==0)
		{
			redirected = 1;
			if(n==len-1)
			{
				printf("Output file is missing.\n Usage -> command >> filename.txt\n");
				return;
			}
			printf("Appended to file %s!!\n",list[n+1]);
			outputFileAt(list,len,n,1);
		}
		
		n++;
	}
	if(redirected==0)
	{
		executeInbuild(list,len);
	}
	if(inrediredted==1)
	{
		freopen("/dev/tty","r",stdin);
	}
}

int chkInpt(char** list,int len)
{
	int i,flg=0;
		if(strcmp(list[0],"ls")==0)
		{
			
			return 1;
		}
	
	
}

int executeCommand (int in, int out, struct command *cmd)
{
	if(chkInpt(cmd->argv,cmd->len)==1)
	{
		if(in != 0)
        {
          dup2 (in, 0);
          close (in);
		  
        }

      	if (out != 1)
        {
          dup2 (out, 1);
          close (out);
        }
		chkFile(cmd->argv,cmd->len);
		return 1;
	}
	pid_t pid =fork();
	if(pid==0)
    {
    	if(in != 0)
        {
          dup2 (in, 0);
          close (in);
        }

      	if (out != 1)
        {
          dup2 (out, 1);
          close (out);
        }

		//chkFile(cmd->argv,cmd->len);
      	if (execvp (cmd->argv [0], (char * const *)cmd->argv) == -1)
      	{
      		fprintf(stderr, "Error executing the intermidiate command\n");
      		exit(EXIT_FAILURE);
      	}
      	return 1;
    }

  return pid;
}

void fork_pipes (int b, struct command *cmd)
{
	//int errLog = open("errLog",O_WRONLY | O_APPEND | O_CREAT,0644);
	//dup2(errLog,2);
  	int i,result;
  	pid_t pid,ppid;
  	int in, fd [2];
  	in = 0;

  	for (i=0;i<b-1;++i)
    {
      pipe (fd);
      executeCommand (in,fd[1],cmd+i);
      close(fd[1]);
      in=fd[0];
    }
  	if (in != 0)
    	dup2 (in, 0);
	pid=fork();
	if(pid==0)
  	{
		  chkFile(cmd[i].argv,cmd[i].len);
	}
  	else
  	{
  		ppid = waitpid(pid,&result,0);
  		while(!WIFEXITED(result) && !WIFSIGNALED(result))
  			ppid = waitpid(pid,&result,0);
  	}
  //dup2(2,errLog);
  //close(errLog);
}

//char **pipe_split_line(char *line)
struct command pipe_split_line(char*line)
{
	int bufsize=BUFFERSIZE_TOKEN, position=0;
	char **tokens=malloc(bufsize*sizeof(char *));
	char *token;
	char * dfgh;
	if(!tokens)
	{
		fprintf(stderr, "Buffer Allocation Error aboarding\n");
		exit(1);
	}
	token=strtok(line, DILIMITOR_WORD);
	while(token!=NULL)
	{
		tokens[position]=token;
		position++;
		if(position>=bufsize)
		{
			bufsize += BUFFERSIZE_TOKEN;
			tokens=realloc(tokens, bufsize*sizeof(char*));
			if(!tokens)
			{
				fprintf(stderr, "Allocation error\n");
				exit(1);
			}
		}
		token=strtok(NULL, DILIMITOR_WORD);
	}
	tokens[position]=NULL;
	struct command cmd ;
	cmd.argv = tokens;
	cmd.len = position;
	return cmd;
}

int pipe_split_command(char* line)
{
	int counter=0;
	int bufsize = BUFFERSIZE_TOKEN, position = 0;
	char *token;
	char *token2;
	char *saveptr;
	char *saveptr2;
	char *list[100];
	struct command cmd[100];
	token = strtok_r(line, DILIMITOR_COMMAND,&saveptr);
	//printf("token1:%s\n",token);

	while (token != NULL)
	{
		token2 = strtok_r(token, DILIMITOR_PIPE,&saveptr2);
	//	printf("%s\n",token2);
		while(token2!=NULL)
		{
			list[counter] = token2;	
			counter++;
			token2 = strtok_r(NULL, DILIMITOR_PIPE,&saveptr2);
		}
		if(counter>1)
			{
				int gg = 0;
				while(gg<counter)
				{
				//	(cmd[gg]).argv = pipe_split_line(list[gg]);
					cmd[gg]=pipe_split_line(list[gg]);
					gg++;
				}

				fork_pipes(counter,cmd);
			}
		token = strtok_r(NULL, DILIMITOR_COMMAND,&saveptr);
		counter=0;
		
	}
	return 1;
}
void HandlingPiping(char str[PATH_MAX])
{
	pipe_split_command(str);
}
void splitcommands(char str[PATH_MAX])
{
	char dilim = '|';
	if(strchr(str,dilim)!=NULL)
	{
		//printf("Has Piping\n");
		HandlingPiping(str);
	}
	else
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
			//executeInbuild(argumentList,looper);
			chkFile(argumentList,looper);

		}
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
	shell_pid = getpid();
	printf("Shell initialized with Process id : %d\n",shell_pid);
	char shellsWD[200];
	getcwd(currentWD,sizeof(currentWD));
	gethostname(deviceName,sizeof(deviceName));
	userName = getUserName();
	char* args;
	signal(SIGINT,ctrlC);
	signal(SIGTSTP,ctrlZ); 
	while(1)
	{
		Childpid=0;
		int a = dup(0);
		int b = dup(1);
		getcwd(shellsWD,sizeof(shellsWD));
		char *cwd1 = shellsWD;
		if(strncmp(currentWD,shellsWD,strlen(currentWD))==0)
				cwd1 = replaceWord(shellsWD,currentWD,"");

		printf("<"YEL"%s@%s"RESET":"BLU"~%s"RESET">\n",userName,deviceName,cwd1);
		//		printf("<%s@%s:~%s>",userName,deviceName,currentWD);
		args = read_command();
		chkAlarm();
		if(strcmp("quit\n",args)==0)
		{
			free(args);
			//printf("status: %d\n",kill(shell_pid,0));
			exit(1);
		}
		splitcommands(args);
		update_job_status();
		free(args);
		dup2(a,0);
		dup2(b,1);
		close(a);
		close(b);
		
	}

}
