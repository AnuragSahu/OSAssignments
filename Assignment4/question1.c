#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>


pthread_t thread_id[2];
int counter;
pthread_mutex_t lock;


void enterAcademy(char character[20])
{
    printf("%s : Entered Academy.\n",character);
}

void meetOrganinser(char character[20])
{
    printf("%s : Met Organisers.\n",character);
}

void enterCourt(char character[20])
{
    printf("%s : Entered court.\n",character);
}

void warmup(char character[20])
{
    printf("%s : Start Warming up.\n",character);
    sleep(1);
    printf("%s : Ended Warming up.\n",character);
}

void adjustEquipment(char character[20])
{
    printf("%s : Adjusting Equipment.\n",character);
    sleep(0.5);
    printf("%s : Adjusted Equipment.\n",character);
}

void startGame(char character[20])
{
    printf("%s : Starting Game.\n",character);
}


void *player_function()
{
    enterAcademy("Player");
    meetOrganinser("Player");
    enterCourt("Player");
    warmup("Player");
}

void *refree_function()
{
    enterAcademy("Refree");
    meetOrganinser("Refree");
    enterCourt("Refree");
    adjustEquipment("Refree");
    startGame("Refree");
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
        }
    }

    printf("Players generated : %d. Refree Generated: %d\n",players,refrees);
    return 0;
}