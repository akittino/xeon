#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
 
#define size 10000
#define INT_MAX (unsigned int)-1
int allNodes[size][size];
 
void generateGraph()
{
        struct timeval tvalBefore;
        struct timeval tvalAfter;
 
        //time_t t;
        int i, j, k, randomData, column, row;
        //srand((unsigned)time(&t));
 
        gettimeofday(&tvalBefore, NULL);
 
        for (i = 0; i < size; i++)
        {
 
                for (j = i; j < size; j++)
                {
                        if (i == j)
                        {
 
                                allNodes[i][j] = 0;
                        }
                        else
                        {
 
                                randomData = rand();
 
                                allNodes[i][j] = randomData;
                                allNodes[j][i] = randomData;
                        }
                }
        }
        column = 0;
        row = 0;
 
        for (k = 0; k < size; k++)
        {
                column = rand() % size;
                row = rand() % size;
                allNodes[column][row] = 0;
                allNodes[row][column] = 0;
 
                //printf("column : %d, row: %d\n", column, row);
        }
 
        gettimeofday(&tvalAfter, NULL);
        printf("Time in microseconds: %ld microseconds\n",
                ((tvalAfter.tv_sec - tvalBefore.tv_sec) * 1000000L
                        + tvalAfter.tv_usec) - tvalBefore.tv_usec
                );
 
}
 
void countDijkstr(int src, int dst)
{
 
        struct timeval tvalBefore;
        struct timeval tvalAfter;  
        unsigned int shortestPath[size];
        char visited[size];
        unsigned int min, indexOfMin;
        int i, j, k, m;
        int numberOfThreads;
 
        gettimeofday(&tvalBefore, NULL);
#pragma omp parallel for
        for (i = 0; i < size; i++)
        {
                shortestPath[i] = INT_MAX;
                visited[i] = 0;
        }
        shortestPath[src] = 0;
 
        for (m = 0; m < size; m++) {
                min = INT_MAX;
 
#pragma omp parallel for
                for (k = 0; k < size; k++)
                {
                        if (visited[k] == 0 && shortestPath[k] <= min)
                        {
                                min = shortestPath[k];
                                indexOfMin = k;
 
                        }
                }
                visited[indexOfMin] = 1;
 
#pragma omp parallel for
                for (j = 0; j < size; j++)
                {
                        if (visited[j] == 0 && allNodes[indexOfMin][j] != 0 && (shortestPath[indexOfMin] + allNodes[indexOfMin][j]) <= shortestPath[j])
                        {
                                shortestPath[j] = allNodes[indexOfMin][j] + shortestPath[indexOfMin];
                        }
                }
        }
 
        gettimeofday(&tvalAfter, NULL);
        printf("Time in microseconds: %ld microseconds\n",
                ((tvalAfter.tv_sec - tvalBefore.tv_sec) * 1000000L
                        + tvalAfter.tv_usec) - tvalBefore.tv_usec
                );
        printf("Shortest Path: %d", shortestPath[dst]);
 
}
 
void printGraph()
{
        int i, j;
        for (i = 0; i < size; i++)
        {
                for (j = 0; j < size; j++)
                {
                        printf("%d ", allNodes[i][j]);
                }
                printf("\n");
        }
}
 
int main()
{
        generateGraph();
        printf("Graph generated\n");
        //printGraph();
 
        countDijkstr(0, size - 1);
 
        return 0;
}