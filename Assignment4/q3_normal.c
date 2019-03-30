//This the Part 2 of Task 3 in Assignment 4 Operating System.
// Implementing merge sort using Threads. 
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
 
#define MAX 10000000

void mergeSort(int [], int, int, int);
void partition(int [],int, int);
 
int main(int argc, char **argv)
{
    
    int list[MAX];
    int i, size;
 
    if(argc>1)
	{
		// taking Input from Command Line Argument.
		size = atoi(argv[1]);
		if(argc - 2 < size)
		{
			printf("Wrong Usage -> ./a.out [lenght of array] [array elements...........].");
			printf("\n*Make sure that the number of array elements is equal to length of array.\n");
			return 0;
		}
		for (int i = 0; i < size; ++i)
			list[i] = atoi(argv[i+2]);
	}
    else
    {
        printf("Enter total number of elements:");
        scanf("%d", &size);
        printf("Enter the elements:\n");
        for(i = 0; i < size; i++)
        {
            scanf("%d", &list[i]);
        }
    }
    partition(list, 0, size - 1);
    printf("After merge sort:\n");
    for(i = 0;i < size; i++)
    {
         printf("%d",list[i]);
         if(i!=size-1)
        {
            printf(", ");
        }
        else{
            printf(".\n");
        }
    }
 
   return 0;
}
 
void partition(int list[],int low,int high)
{
    int mid;
 
    if(low < high)
    {
        mid = (low + high) / 2;
        partition(list, low, mid);
        partition(list, mid + 1, high);
        mergeSort(list, low, mid, high);
    }
}
 
void mergeSort(int list[],int low,int mid,int high)
{
    int i, mi, k, lo, temp[MAX];
 
    lo = low;
    i = low;
    mi = mid + 1;
    while ((lo <= mid) && (mi <= high))
    {
        if (list[lo] <= list[mi])
        {
            temp[i] = list[lo];
            lo++;
        }
        else
        {
            temp[i] = list[mi];
            mi++;
        }
        i++;
    }
    if (lo > mid)
    {
        for (k = mi; k <= high; k++)
        {
            temp[i] = list[k];
            i++;
        }
    }
    else
    {
        for (k = lo; k <= mid; k++)
        {
             temp[i] = list[k];
             i++;
        }
    }
 
    for (k = low; k <= high; k++)
    {
        list[k] = temp[k];
    }
}