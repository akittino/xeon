// scp mst.c student01@apl12.eti.pg.gda.pl:~/kantee
// source /opt/intel/composer_xe_2013_sp1.3.174/bin/compilervars.sh intel64
// icc -openmp -mmic -O3 mst.c -o mst
// scp mst mic0:~
// scp /opt/intel/composer_xe_2013_sp1.3.174/compiler/lib/mic/libiomp5.so mic0:~
// export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
// time ./mst

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

typedef short unsigned int dint;

#define SIZE 1000
#define MAX (dint)-1
#define TRUE 1

dint data[SIZE][SIZE];
short got[SIZE];

void generateGraph()
{
    unsigned ii, jj;
//#pragma omp parallel for // - not used to have always the same data
// unused
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
    //#pragma omp parallel for reduction(+:total)
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

    //#pragma omp parallel
    {
      unsigned lmin = MAX;
      unsigned lx = 0, ly = 0;

      //#pragma omp for nowait
      for (ii = 0; ii < SIZE; ++ii)
      {
	      for (jj = 0; jj < SIZE; ++jj)
	      {
		      if (data[ii][jj] < lmin)
		      {
			      lmin = data[ii][jj];
			      lx = ii;
			      ly = jj;
		      }
	      }
      }

      //#pragma omp critical
      {
	if(lmin < minimum)
	{
	  minimum = lmin;
	  x = lx;
	  y = ly;
	}
      }
    }
    mst += data[x][y];
    got[x] = TRUE;
    got[y] = TRUE;
    minimum = MAX;
    //printf("%u %u\n", x + 1, y + 1);

    while (!gotAll())
    {
  //#pragma omp parallel
	{
	unsigned lmin = MAX;
	unsigned lx = 0, ly = 0;

	//#pragma omp for nowait
	for (ii = 0; ii < SIZE; ++ii)
	{
		for (jj = 0; jj < SIZE; ++jj)
		{
		    if ((got[ii] ^ got[jj]) && (data[ii][jj] < lmin))
		    {
			    lmin = data[ii][jj];
			    lx = ii;
			    ly = jj;
		    }
		}
	}

	//#pragma omp critical
	{
	  if(lmin < minimum)
	  {
	    minimum = lmin;
	    x = lx;
	    y = ly;
	  }
	}
  printf("min:%u", minimum);
      }
      mst += data[x][y];
      got[x] = TRUE;
      got[y] = TRUE;
      minimum = MAX;
      //printf("%u %u\n", x + 1, y + 1);
    }
    printf("%llu", mst);
}
