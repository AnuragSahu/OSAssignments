// This is a code reffered from an article of Geeks-For-Geeks
// Contributed by Kisley Verma
// Link:- https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
// In order to compile it -> gcc thread_syncronisation.c -lpthread
//

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>

pthread_t thread_id[2];
int counter;
pthread_mutex_t lock;

void* tryThis(void * arg)
{
	pthread_mutex_lock(&lock);

	unsigned long i = 0;
	counter += 1;
	printf("\n Job %d has Started\n",counter);

	for(i=0;i<(0xFFFFFFFF);i++);

	printf("\n Job %d has finished.\n",counter);

	pthread_mutex_unlock(&lock);

	return NULL;

}

int main(void)
{
	int i=0;
	int error;
	if(pthread_mutex_init(&lock, NULL)!=0)
	{
		printf("\n Mutex Unable to be inititalzed\n");
		return 1;
	}

	while(i<2)
	{
		error = pthread_create(&(thread_id[i]),NULL,&tryThis, NULL);
		if(error!=0)
			printf("\n THread can't be created: [%s]",strerror(error));
		i++;
	}

	pthread_join(thread_id[0],NULL);
	pthread_join(thread_id[1],NULL);
	pthread_mutex_destroy(&lock);

	return 0;
}


