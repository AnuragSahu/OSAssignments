#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>

int playerWithOrganiser = 0; //Number of player with organiser
int refreeWithOrganiser = 0; //Number of refree with organiser

pthread_mutex_t playerMeetOrganiserMutex;
pthread_mutex_t meetOrganiserMutex;
pthread_mutex_t teamFormationMutex;
pthread_mutex_t enterCourtMutex;
pthread_cond_t condPlayer;
pthread_cond_t condRefree;
pthread_cond_t condEnterCourt;


int flag = 0;
int counter;
pthread_t thread_id[2];
pthread_mutex_t lock;


void teamFormation(char name[20])
{
    pthread_mutex_lock(&teamFormationMutex);
    printf("%s: Waiting for Team Formation.\n",name);
    while(flag==0){
        printf("%s , waiting.\n",name);
        pthread_cond_wait(&condEnterCourt,&teamFormationMutex);
    }
    if(refreeWithOrganiser==0 && playerWithOrganiser==0)
    {
        printf("Orgaiser Free.\n");
    }
    printf("%s: Team is Formed.\n",name);
    if(memcmp(name,"Refree",6)==0)
    {
        flag = 0;
    }
    pthread_mutex_unlock(&teamFormationMutex);
    
}


void enterCourt(char name[20])
{
    pthread_mutex_lock(&enterCourtMutex);
    printf("%s: Entered Court.\n",name);
    pthread_mutex_unlock(&enterCourtMutex);
    
}

void meetOrganiser(char name[20])
{
    pthread_mutex_lock(&meetOrganiserMutex);
    if(memcmp(name,"Player",6)==0)
    {
        if(playerWithOrganiser==2)
        {
           printf("%s: Waiting for Organiser.\n",name);
        }
        while(playerWithOrganiser==2)
        {
            pthread_cond_wait(&condPlayer,&meetOrganiserMutex);
        }
        if(playerWithOrganiser<2)
        {
           playerWithOrganiser+=1; 
           printf("%s: met organiser.\n",name);       
        }
    }
    else if(memcmp(name,"Refree",6)==0)
    {
        if(refreeWithOrganiser==1)
        { 
            printf("%s: Waiting for Organiser.\n",name);
        }
        while(refreeWithOrganiser==1)
        {
            pthread_cond_wait(&condRefree,&meetOrganiserMutex); 
        }
         if(refreeWithOrganiser==0)
        {
            refreeWithOrganiser+=1;
            printf("%s: met organiser.\n",name);
        }
    }
    if(refreeWithOrganiser==1 && playerWithOrganiser==2 && (memcmp(name,"Refree",6)==0))
        {
            
            pthread_cond_broadcast(&condEnterCourt);
            flag = 1;
            //printf("%s is opening the lock.\n",name);
            pthread_cond_signal(&condPlayer);
            //pthread_cond_signal(&condPlayer);
            pthread_cond_signal(&condRefree);
            refreeWithOrganiser = 0;
            playerWithOrganiser = 0;
            
        }
   pthread_mutex_unlock(&meetOrganiserMutex);
}

void enterAcademy(char name[20])
{
    printf("%s: Entered Academy\n",name);
}

void* refree(int id)
{
    char refreeid[20];
    sprintf(refreeid,"Refree%d",id);
    enterAcademy(refreeid);
    meetOrganiser(refreeid);
    teamFormation(refreeid);
    enterCourt(refreeid);
    return  NULL;
}



void* player(int id)
{
    char playerid[20];
    sprintf(playerid,"Player%d",id);
    enterAcademy(playerid);
    meetOrganiser(playerid);
    teamFormation(playerid);
    enterCourt(playerid);
    return  NULL;
}

//void* organiser(){}

int main()
{
    pthread_mutex_init(&playerMeetOrganiserMutex,NULL);
    pthread_mutex_init(&meetOrganiserMutex,NULL);
    pthread_mutex_init(&enterCourtMutex,NULL);
    
    pthread_cond_init(&condPlayer, NULL);
    pthread_cond_init(&condRefree, NULL);
    pthread_cond_init(&condEnterCourt, NULL);

    int n;
    printf("Enter n : ");
    scanf("%d",&n);
    pthread_t* player_threads = (pthread_t*)malloc(2*n*sizeof(pthread_t));
    pthread_t* refree_threads = (pthread_t*)malloc(n*sizeof(pthread_t));
    pthread_t organiser_thread;
    int* player_id = (int*)malloc(2*n*sizeof(int));
    int* refree_id = (int*)malloc(n*sizeof(int));
    //pthread_create(&organiser_thread,NULL,organiser,NULL);
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
        //printf("%lf\n",probability_of_player);
        if(probability_of_player < 5 && no_refree > 0)
        {
            refrees++;
            refree_id[refrees-1]=refrees;
            flg=1;
            //printf("Generating refree\n");
            no_refree--;
            pthread_create(&refree_threads[refrees],NULL,refree,refree_id[refrees-1]);
            
            
        }
        else if(probability_of_player >= 5 && no_player > 0)
        {
            players++;
            flg=1;
            player_id[players-1]= players;
            //printf("Generating Player\n");
            no_player--;
            pthread_create(&player_threads[players],NULL,player,player_id[players-1]);
            
        }
        if(flg==0)
            i--;
        sleep(rand()%3);
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