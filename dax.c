#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

#define SIZE 10000
#define INT_MAX (unsigned int)-1
#define NUMBER_OF_THREADS 32

int allNodes[SIZE][SIZE];

void generateGraph()
{
  struct timeval tvalBefore;
  struct timeval tvalAfter;
  //time_t t;
  int i, j, k, randomData, column, row;
  //srand((unsigned)time(&t));

  gettimeofday(&tvalBefore, NULL);

  for (i = 0; i < SIZE; i++)
  {
    for (j = i; j < SIZE; j++)
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

  for (k = 0; k < SIZE; k++)
  {
    column = rand() % SIZE;
    row = rand() % SIZE;
    allNodes[column][row] = 0;
    allNodes[row][column] = 0;
    //printf("column : %d, row: %d\n", column, row);
  }

  gettimeofday(&tvalAfter, NULL);
  printf("Time in microseconds: %ld microseconds\n",
  ((tvalAfter.tv_sec - tvalBefore.tv_sec) * 1000000L + tvalAfter.tv_usec) - tvalBefore.tv_usec);
}

void countDijkstra(int src, int dst)
{
  struct timeval tvalBefore;
  struct timeval tvalAfter;
  unsigned int shortestPath[SIZE], localMins[NUMBER_OF_THREADS], localIndexOfMins[NUMBER_OF_THREADS];
  char visited[SIZE];
  unsigned int min = 0, indexOfMin = 0, tid = 0;
  int i, j, k, m;

  omp_set_num_threads(NUMBER_OF_THREADS);
  gettimeofday(&tvalBefore, NULL);
  //  #pragma omp parallel for private(i) shared(shortestPath, visited)
  #pragma simd
  for (i = 0; i < SIZE; i += 4)
  {
    shortestPath[i] = INT_MAX;
    shortestPath[i + 1] = INT_MAX;
    shortestPath[i + 2] = INT_MAX;
    shortestPath[i + 3] = INT_MAX;
    visited[i] = 0;
    visited[i + 1] = 0;
    visited[i + 2] = 0;
    visited[i + 3] = 0;
  }
  shortestPath[src] = 0;

  for (m = 0; m < SIZE; m++)
  {
    min = INT_MAX;
    #pragma simd
    for (i = 0; i < NUMBER_OF_THREADS; i+=8)
    {
      localMins[i] = INT_MAX;
      localMins[i+1] = INT_MAX;
      localMins[i+2] = INT_MAX;
      localMins[i+3] = INT_MAX;
      localMins[i+4] = INT_MAX;
      localMins[i+5] = INT_MAX;
      localMins[i+6] = INT_MAX;
      localMins[i+7] = INT_MAX;
    }
    #pragma omp parallel shared(visited, shortestPath, localIndexOfMins, localMins) private(k, tid)
    {
      tid = omp_get_thread_num();
      #pragma omp for nowait
      for (k = 0; k < SIZE; k++)
      {
        if (visited[k] == 0 && shortestPath[k] <= localMins[tid])
        {
          localMins[tid] = shortestPath[k];
          localIndexOfMins[tid] = k;
        }
      }
    }

    for (i = 0; i < NUMBER_OF_THREADS; i++)
    {
      if (localMins[i] < min)
      {
        min = localMins[i];
        indexOfMin = localIndexOfMins[i];
      }
    }
    visited[indexOfMin] = 1;

    #pragma omp parallel for private(j) shared(allNodes, shortestPath, visited)
    for (j = 0; j < SIZE; j++)
    {
      if (visited[j] == 0 && allNodes[indexOfMin][j] != 0 && (shortestPath[indexOfMin] + allNodes[indexOfMin][j]) <= shortestPath[j])
      {
        shortestPath[j] = allNodes[indexOfMin][j] + shortestPath[indexOfMin];
      }
    }
  }

  gettimeofday(&tvalAfter, NULL);
  printf("Time in microseconds: %ld microseconds\n",
  ((tvalAfter.tv_sec - tvalBefore.tv_sec) * 1000000L + tvalAfter.tv_usec) - tvalBefore.tv_usec);
  printf("Shortest Path: %d\n", shortestPath[dst]);
}

void printGraph()
{
  int i, j;
  for (i = 0; i < SIZE; i++)
  {
    for (j = 0; j < SIZE; j++)
    {
      printf("%d ", allNodes[i][j]);
    }
    printf("\n");
  }
}

int main()
{
  printf("GRAPH SIZE: %d\n", SIZE);
  generateGraph();
  printf("Graph generated\n");
  //printGraph();
  countDijkstra(0, SIZE - 1);

  return 0;
}
