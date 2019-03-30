#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>

int n;
int *shArray;

key_t key = IPC_PRIVATE;

void merge_sort(int left, int right);
void merge(int left, int mid, int right);
int main(int argc, char **argv)
{
	if(argc>1)
	{
		// taking Input from Command Line Argument.
		n = atoi(argv[1]);
		if(argc - 2 < n)
		{
			printf("Wrong Usage -> ./a.out [lenght of array] [array elements...........].");
			printf("\n*Make sure that the number of array elements is equal to length of array.\n");
			return 0;
		}
		int shmid = shmget(key, sizeof(int)*n, IPC_CREAT| 0666);
		if(shmid == -1){
			perror("Shmget failed");
			exit(1);
		}
		shArray = shmat(shmid, 0, 0);
		if(shArray == (void *)-1) {
			perror("Shmat failed");
			exit(1);
		}
		for (int i = 0; i < n; ++i)
			shArray[i] = atoi(argv[i+2]);
	}
	else
	{
		printf("Enter n: ");
		scanf("%d",&n);
		int shmid = shmget(key, sizeof(int)*n, IPC_CREAT| 0666);
		if(shmid == -1){
			perror("Shmget failed");
			exit(1);
		}
		shArray = shmat(shmid, 0, 0);
		if(shArray == (void *)-1) {
			perror("Shmat failed");
			exit(1);
		}
		printf("Enter the shArrayay Elements: ");
		for (int i = 0; i < n; ++i)
			scanf("%d",&shArray[i]);
	}

	merge_sort(0,n-1);

	printf("After sort\n");
	for (int i = 0; i <n ; ++i)
	{
		printf("%d",shArray[i]);	
		if(i!=n-1)
        {
            printf(", ");
        }
        else{
            printf(".\n");
        }
	}
	return 0;
}


void merge_sort(int left, int right){

	if(right - left < 2)
	{
		int p1,p2,min_idx;
		for (p1 = left; p1 < right; p1++){
        	min_idx = p1;
	        for (p2 = p1+1; p2 <=right; p2++)
	          if (shArray[p2] < shArray[min_idx])
	            min_idx = p2;
        	shArray[min_idx] = shArray[min_idx] + shArray[p1] - (shArray[p1] = shArray[min_idx]);
    	}

	}

	else
	{
		int mid = left + (right-left)/2;

		pid_t pid = fork();

		if(pid<0) {
		perror("fork Failed");
		exit(1);
		} 
		if(pid == 0){
			merge_sort(left,mid);
			exit(0);
		}
		else{
			wait(NULL);
			merge_sort(mid+1,right);
		}
		merge(left,mid,right);
	}
}

void merge(int left, int mid, int right){

	int low = mid - left + 1;
	int high = right - mid;

	int shArray1[low],shArray2[high];

	for (int i = left; i <=mid ; ++i)
		shArray1[i - left] = shArray[i];	

	for (int i = mid+1; i <=right ; ++i)
		shArray2[i - mid - 1] = shArray[i];

	int i=0,j=0,k=left;

	while(i < low && j< high)
		if(shArray1[i]<shArray2[j])
			shArray[k++]=shArray1[i++];
		else
			shArray[k++]=shArray2[j++];

	while(i < low)
		shArray[k++] = shArray1[i++];

	while(j < high)
		shArray[k++] = shArray2[j++];
}