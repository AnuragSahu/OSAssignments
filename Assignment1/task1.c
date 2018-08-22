#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

void reverse(char str1[], int index, int size)   // Here this is a recursive function for reversing the array
{
    char temp;
    temp = str1[index];
    str1[index] = str1[size - index];
    str1[size - index] = temp;
    if (index == size / 2)
    {
        return;							//	Return if reached the mid of array
    }
    reverse(str1, index + 1, size);
}
int len(char* a)						// 	This function gives Length og the array used in printfor me function
{
        int len=0;
        for(;a[len]!='\0';++len);
        return len;
}

void printforme(char* string)					// 	this functions prints the given string through SYSTEM CALL write
{
        write(1,string,len(string));
}

int main(int argc, char *argv[])
{      
        int fd_Read, fd_Write;
 	double ofset;
	char ch;
	int writeFileDes;
	int bufrSize = 10000;					// Change the buffer Size for optimization
	if(argc<2)
	{
		printforme("Enter the file name also like : ./a.out fileName \n");		// No NAme has been Provided to ./a.out
		return 0;
	}
	


        int readFileDes = open(argv[1],O_RDONLY);
	if(readFileDes<0) 
	{
		printforme(" Not able to open the file \n");		//	Incase the file ain't found
		return 0;
	}
	int fileDir = mkdir("Assignment",S_IRWXU);
	if(fileDir<0)
	{
		rmdir("Assignment/rev1.txt");
		writeFileDes = open("Assignment/rev1.txt",O_RDWR);
		if(writeFileDes<0)
			writeFileDes = creat("Assignment/rev1.txt",S_IRUSR|S_IWUSR);		//		printf("Assignment Directory is already present\n");
	}
	else
	{
		writeFileDes = creat("Assignment/rev1.txt",S_IRUSR|S_IWUSR);			//	       	printf("Created Directory Assignment\n");
	}

	char* bufr = (char*)malloc(bufrSize*sizeof(char));    		//	Allocating Mem
	off_t a = lseek(readFileDes,0,SEEK_END);
//	int a = read(readFileDes,bufr,bufrSize);				//	Reading
	ssize_t b=0;
	int i,flg=1;
	while(flg)
	{
		a = lseek(readFileDes,-(bufrSize+b),SEEK_CUR);
		if(a<0)
		{
			a = lseek(readFileDes,-bufrSize,SEEK_CUR);
			i=bufrSize-1;
			for(;i>0;i--)
				if(lseek(readFileDes,-i,SEEK_CUR)!=-1)
				{
					bufrSize = i;
					break;
				}
			flg--;
		}

		b = read(readFileDes,bufr,bufrSize);
		if(flg==0)
			b=bufrSize;
		for(i=0;i<b;i++){
			bufr[i] = (bufr[i]>='a' && bufr[i]<='z')?bufr[i]-=32 : (bufr[i]>='A' && bufr[i]<='Z')? bufr[i]+=32 : bufr[i];
		}
		reverse(bufr,0,b-1);
//		printf("%s",bufr);
		write(writeFileDes,bufr,b);
	}
//	printf("|\n");

/*CHarachter by character reading	
	char c;
	ofset = lseek(fileDescribtor,-1,SEEK_END);
	while(ofset>0)
	{
		read(fileDescribtor,&c,1);
		c = (c>='a' && c<='z')?c-=32:(c>='A' && c<='Z')?c+=32:c;
		printf("%c",c);
		write(fileDescribtor1,&c,1);
		lseek(fileDescribtor,-2,SEEK_CUR);
		ofset--;

	}
	lseek(fileDescribtor1,0,SEEK_SET);
*/


	printforme("Reversed the File you can see it at Assignment/rev1.txt\n");
//----------------------------------------------------------Now For Task 2------------------------------------------------------
/*	
	if(symlink("Assignment","Assignment_symlink")<0)
	{
		printf("Unable to Create a Symlink,\n");
	       printf("There maybe another SymLink already present of the name Assignment_symlink\n");
	}
	
	char*  ques21= "\nChecking whether the directory has been created: ";
	int len ;
	for(len = 0; ques21[len] != '\0'; ++len);
	printf("%d\n",len);
	write(1,ques21,len);
	if(access("Assignment",F_OK)!=-1)
		printf("Yes\n");
	else 
		printf("No\n");


	printf("Checking whether the file has been created: ");
	if(access("Assignment/rev1.txt",F_OK)!=-1)
		printf("Yes\n");
	else 
		printf("No\n");

	printf("Checking whether the symlink has been created: ");
	if(access("Assignment_symlink",F_OK)!=-1)
		printf("Yes\n");
	else 
		printf("No\n");


	printf("Checking whether file contents have been reversed and case-inverted: ");
	if(access("Assignment_symlink/rev1.txt",F_OK)!=-1)
		printf("Yes\n");
	else 
		printf("No\n");
	struct stat fileStat;
	if(stat("Assignment_symlink",&fileStat)<0)
	{
		printf("Unable to access stats of Assignment_Synlink File\n");
		return 0;
	}

	printf("\n\nUser has read permission on file: ");
	printf( (fileStat.st_mode & S_IRUSR) ? "Yes\n" : "No\n");
	
	printf("User has write permission on file: ");
	printf( (fileStat.st_mode & S_IWUSR) ? "Yes\n" : "No\n");
	
	printf("User has execute permission on file: ");
	printf( (fileStat.st_mode & S_IXUSR) ? "Yes\n" : "No\n");

	
	printf("\n\nGroup has read permission on file: ");
	printf( (fileStat.st_mode & S_IRGRP) ? "Yes\n" : "No\n");
	
	printf("Group has write permission on file: ");
	printf( (fileStat.st_mode & S_IWGRP) ? "Yes\n" : "No\n");
	
	printf("Group has execute permission on file: ");
	printf( (fileStat.st_mode & S_IXGRP) ? "Yes\n" : "No\n");

	
	printf("\n\nOthers has read permission on file: ");
	printf( (fileStat.st_mode & S_IROTH) ? "Yes\n" : "No\n");
	
	printf("Others has write permission on file: ");
	printf( (fileStat.st_mode & S_IWOTH) ? "Yes\n" : "No\n");
	
	printf("Others has execute permission on file: ");
	printf( (fileStat.st_mode & S_IXOTH) ? "Yes\n" : "No\n");
*/


	close(readFileDes);
	close(writeFileDes);
	return 0;
}
