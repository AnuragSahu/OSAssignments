#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int len(char* a)
{
	int len=0;
	for(;a[len]!='\0';++len);
	return len;
}

int fileExists(const char* file) {
    struct stat buf;
    if(stat(file, &buf) == 0)
	    return 1;
    else return 0;
}

void printRes(int a)
{

	char* yes = "Yes\n";
	char* no = "No\n";

	if(a==1)	write(1,yes,len(yes));
	else write(1,no,len(no));
}

char invChar(char bufr)
{
	return (bufr>='a' && bufr<='z')?bufr-=32 : (bufr>='A' && bufr<='Z')? bufr+=32 : bufr;
}

int chkFileCorrectness(int testFileDes, int resFileDes)
{
	int result = 1;
	char resChar,testChar;
	off_t a = lseek(testFileDes,-1,SEEK_END);
	while(a>0)
	{
		read(resFileDes,&resChar,1);
		read(testFileDes,&testChar,1);
		a = lseek(testFileDes,-2,SEEK_CUR);
		if(testChar != invChar(resChar))
		{
			printf("%c,%c\n",testChar,resChar);
			return 0;
		}
	}
	return 1;
}

void printforme(char* string)
{
	write(1,string,len(string));
}

int main(int argc, char* argv[])
{
	if(argc<2)
        {
                printf("Enter the file name to check reverse like : ./a.out fileName \n");
                return 0;
        }


	if(symlink("Assignment","Assignment_symlink")<0)
        {
                printforme("Unable to Create a Symlink,\n");
               printforme("There maybe another SymLink already present of the name Assignment_symlink\n");
        }

	printforme("\nChecking whether the directory has been created: ");
        printRes(fileExists("Assignment")); 


        printforme("Checking whether the file has been created: ");
        printRes(fileExists("Assignment/rev1.txt"));

        printforme("Checking whether the symlink has been created: ");
        printRes(fileExists("Assignment_symlink"));


 	printforme("Checking whether file contents have been reversed and case-inverted: ");
 	int testFileDes = open(argv[1],O_RDONLY);
	int resFileDes = open("Assignment/rev1.txt",O_RDONLY);
 	printRes(chkFileCorrectness(testFileDes,resFileDes));
        
	struct stat fileStat;
        if(stat("Assignment/rev1.txt",&fileStat)<0)
        {
                printforme("Unable to access stats of Assignment/rev1.txt File\n");
                return 0;
        }

	printforme("\n\nThese are the permissions for File: Assignment/rev1.txt \n");

        printforme("\nUser has read permission on file: ");
        printforme( (fileStat.st_mode & S_IRUSR) ? "Yes\n" : "No\n");

        printforme("User has write permission on file: ");
        printforme( (fileStat.st_mode & S_IWUSR) ? "Yes\n" : "No\n");

        printforme("User has execute permission on file: ");
        printforme( (fileStat.st_mode & S_IXUSR) ? "Yes\n" : "No\n");


        printforme("\n\nGroup has read permission on file: ");
        printforme( (fileStat.st_mode & S_IRGRP) ? "Yes\n" : "No\n");

        printforme("Group has write permission on file: ");
        printforme( (fileStat.st_mode & S_IWGRP) ? "Yes\n" : "No\n");

        printforme("Group has execute permission on file: ");
        printforme( (fileStat.st_mode & S_IXGRP) ? "Yes\n" : "No\n");


        printforme("\n\nOthers has read permission on file: ");
        printforme( (fileStat.st_mode & S_IROTH) ? "Yes\n" : "No\n");

        printforme("Others has write permission on file: ");
        printforme( (fileStat.st_mode & S_IWOTH) ? "Yes\n" : "No\n");

        printforme("Others has execute permission on file: ");
        printforme( (fileStat.st_mode & S_IXOTH) ? "Yes\n" : "No\n");
        
	struct stat dirStat;
        if(stat("Assignment",&dirStat)<0)
        {
                printforme("Unable to access stats of Assignment Directory\n");
                return 0;
        }

	printforme("\n\nThese are the permissions for File: Assignment Directory \n");

        printforme("\nUser has read permission on file: ");
        printforme( (dirStat.st_mode & S_IRUSR) ? "Yes\n" : "No\n");

        printforme("User has write permission on file: ");
        printforme( (dirStat.st_mode & S_IWUSR) ? "Yes\n" : "No\n");

        printforme("User has execute permission on file: ");
        printforme( (dirStat.st_mode & S_IXUSR) ? "Yes\n" : "No\n");


        printforme("\n\nGroup has read permission on file: ");
        printforme( (dirStat.st_mode & S_IRGRP) ? "Yes\n" : "No\n");

        printforme("Group has write permission on file: ");
        printforme( (dirStat.st_mode & S_IWGRP) ? "Yes\n" : "No\n");

        printforme("Group has execute permission on file: ");
        printforme( (dirStat.st_mode & S_IXGRP) ? "Yes\n" : "No\n");


        printforme("\n\nOthers has read permission on file: ");
        printforme( (dirStat.st_mode & S_IROTH) ? "Yes\n" : "No\n");

        printforme("Others has write permission on file: ");
        printforme( (dirStat.st_mode & S_IWOTH) ? "Yes\n" : "No\n");

        printforme("Others has execute permission on file: ");
        printforme( (dirStat.st_mode & S_IXOTH) ? "Yes\n" : "No\n");
        
}
