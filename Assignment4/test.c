#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>


pthread_t thread_id[2];
int counter;
pthread_mutex_t lock;


void* refree()
{
    printf("Refree Refreeing\n");
}


void* player()
{
    printf("Player playing\n");
}

int main()
{
    int n;
    printf("Enter n : ");
    scanf("%d",&n);
    pthread_t* player_threads = (pthread_t*)malloc(2*n*sizeof(pthread_t));
    pthread_t* refree_threads = (pthread_t*)malloc(n*sizeof(pthread_t));
    if(pthread_mutex_init(&lock, NULL)!=0)
    {
        printf("\n Mutex unable to initialzed\n");
        return 1;
    }
    int no_player = 2*n;
    int no_refree = n;
    int players = 0;
    int refrees = 0;
    int flg=0;
    //pthread_t thread_id;
    double probability_of_player;
    for(int i=0;i<3*n;i++)
    {
        probability_of_player = ((double)no_player / ((double)no_player + (double)no_refree))*10;
        int prob = rand()%10;
        int a =(int)probability_of_player % 10;
        if(a < 5 && no_refree > 0)
        {
            flg=1;
            //printf("Generating refree\n");
            no_refree--;
            pthread_create(&refree_threads[refrees],NULL,refree,NULL);
            refrees++;
            
        }
        else if(a >= 5 && no_player > 0)
        {
            flg=1;
            //printf("Generating Player\n");
            no_player--;
            pthread_create(&player_threads[players],NULL,player,NULL);
            players++;
        }
        if(flg==0)
            i--;
    }

    for(int i=0;i<2*n;i++)
    {
        if(i<n)
        {
            pthread_join(refree_threads[i],NULL);
        }
        pthread_join(player_threads[i],NULL);
    }

    printf("Players generated : %d. \nRefree Generated: %d\n",players,refrees);
    return 0;
}