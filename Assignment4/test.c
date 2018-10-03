#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>


pthread_t thread_id[2];
int counter;
pthread_mutex_t lock;


void* player()
{
    printf("Player playing\n");
}


int main()
{
    int n;
    printf("Enter n : ");
    scanf("%d",&n);
    if(pthread_mutex_init(&lock, NULL)!=0)
    {
        printf("\n Mutex unable to initialzed\n");
        return 1;
    }
    int no_player = 2*n;
    int no_refree = n;
    int players = 0;
    int refrees = 0;
    pthread_t thread_id;
    double probability_of_player;
    for(int i=0;i<3*n;i++)
    {
        probability_of_player = ((double)no_player / ((double)no_player + (double)no_refree))*10;
        int prob = rand()%10;
        int a =(int)probability_of_player % 10;
        if(prob > a && no_refree > 0)
        {
            printf("Generating refree\n");
            no_refree--;
            refrees++;
            
        }
        else if(prob <= a && no_player > 0)
        {
            printf("Generating Player\n");
            no_player--;
            players++;
            pthread_create(&thread_id,NULL,player,NULL);
        }
    }

    printf("Players generated : %d. \nRefree Generated: %d\n",players,refrees);
    return 0;
}