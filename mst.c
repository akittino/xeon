/*
scp mst.c student01@apl12.eti.pg.gda.pl:~/kantee
source /opt/intel/composer_xe_2013_sp1.3.174/bin/compilervars.sh intel64
icc -openmp -mmic -O3 mst.c -o mst
scp mst mic0:~
scp /opt/intel/composer_xe_2013_sp1.3.174/compiler/lib/mic/libiomp5.so mic0:~
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
time ./mst
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

typedef short unsigned int dint;

#define SIZE 1000
#define MAX (dint)-1
#define TRUE 1
#define THREADS 240

dint data[SIZE][SIZE];
short got[SIZE];

void generateGraph()
{
  unsigned ii, jj;
  for (ii = 0; ii < SIZE; ++ii)
  {
    for (jj = 0; jj < SIZE; ++jj)
    {
      if (ii == jj)
      {
        data[ii][jj] = data[jj][ii] = MAX;
      }
      else
      {
        dint tmp = (dint)rand() % MAX;
        data[ii][jj] = data[jj][ii] = tmp;
      }
    }
  }
}

void printGraph()
{
  unsigned ii, jj;
  for (ii = 0; ii < SIZE; ++ii)
  {
    for (jj = 0; jj < SIZE; ++jj)
    {
      if(data[ii][jj] == MAX)
      printf("%u ", 0);
      else
      printf("%u ", data[ii][jj]);
    }
    printf("\n");
  }
}

unsigned gotAll()
{
  unsigned total = 0;
  unsigned ii;
  #pragma omp parallel for reduction(+:total) shared(got) private(ii)
  for (ii = 0; ii < SIZE; ++ii)
  {
    if (got[ii] == TRUE)
    total += 1;
  }
  //printf("%u\n", total);
  return total == SIZE;
}

int main()
{
  dint minimum = MAX;
  unsigned x = 0, y = 0;
  long long unsigned int mst = 0;
  unsigned ii, jj;
  generateGraph();
  //printGraph();
  omp_set_num_threads(THREADS);
  unsigned lmin[THREADS] = {0};
  unsigned lx[THREADS] = {0};
  unsigned ly[THREADS] = {0};

  for(ii = 0; ii < THREADS; ++ii)
  {
    lmin[ii] = MAX;
  }

  #pragma omp parallel shared(data, lmin, lx, ly) private(ii, jj)
  {
    unsigned tid = omp_get_thread_num();

    #pragma omp for nowait
    for (ii = 0; ii < SIZE; ++ii)
    {
      for (jj = 0; jj < SIZE; ++jj)
      {
        if (data[ii][jj] < lmin[tid])
        {
          lmin[tid] = data[ii][jj];
          lx[tid] = ii;
          ly[tid] = jj;
        }
      } // for jj
    } // for ii
  } // pragma omp parallel

  // find global minimum from local minimums
  for(ii = 0; ii < THREADS; ++ii)
  {
    if(lmin[ii] < minimum)
    {
      minimum = lmin[ii];
      x = lx[ii];
      y = ly[ii];
    }
  }

  mst += data[x][y];
  got[x] = TRUE;
  got[y] = TRUE;
  minimum = MAX;
  //printf("%u %u\n", x + 1, y + 1);

  while (!gotAll())
  {
    for(ii = 0; ii < THREADS; ++ii)
    {
      lmin[ii] = MAX;
    }

    #pragma omp parallel shared(data, got, lmin, lx, ly) private(ii, jj)
    {
      unsigned tid = omp_get_thread_num();

      #pragma omp for nowait
      for (ii = 0; ii < SIZE; ++ii)
      {
        for (jj = 0; jj < SIZE; ++jj)
        {
          if ((got[ii] ^ got[jj]) && (data[ii][jj] < lmin[tid]))
          {
            lmin[tid] = data[ii][jj];
            lx[tid] = ii;
            ly[tid] = jj;
          }
        } // for jj
      } // for ii
    } // pragma omp parallel

    // find global minimum from local minimums
    for(ii = 0; ii < THREADS; ++ii)
    {
      if(lmin[ii] < minimum)
      {
        minimum = lmin[ii];
        x = lx[ii];
        y = ly[ii];
      }
    }

    mst += data[x][y];
    got[x] = TRUE;
    got[y] = TRUE;
    minimum = MAX;
    //printf("%u %u\n", x + 1, y + 1);
  }
  printf("%llu", mst);
}
