#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

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
    pthread_t  person_thread_id;
    int number_of_players = 2*n;
    int number_of_refrees = n;
    //printf("Number of players entering Academy : %d\n",number_of_players);
    //printf("Number of refrees entering Academy : %d\n",number_of_refrees);
    for(int i=0;i<3*n;i++)
    {
        sleep(rand()%3);
        
        //printf("HEllo\n");
    }
    return 0;
}