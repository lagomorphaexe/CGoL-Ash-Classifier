#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bits.h"

char charval(int a)
{
  if(a == 0) return 'b';
  if(a == 1) return 'o';
  return '?';
}

int main( int argc, char *argv[] )
{
  /* CMDLINE PROCESSING */
  char *fname;
  int w = 50, h = 50;
  if( argc == 2 )
  {
    fname = argv[1];
  }
  else if( argc == 4 )
  {
    printf("%s\n", argv[1]);
    fname = argv[1];
    w = atoi(argv[2]);
    h = atoi(argv[3]);
  }
  else
  {
    printf("Incorrect number of arguments. Use:\n");
    printf("  ./genGrid [file]\n");
    printf("  ./genGrid [file] [width] [height]\n");
    return 0;
  }

  printf("Creating %d x %d random fill RLE\n", w, h);

  /* RANDOM SEEDING */
  srand(time(NULL));

  FILE *f;
  f = fopen(fname, "w+");

  fprintf(f, "x = %d, y = %d, rule = B3/S23\n", w, h);
  
  for(int y = 0; y < h; ++y)
  {
    int count = 1;
    int prev = -1;
    if(y % pown(10, ilog(h, 10) - 4) == 0)
    {
      printProgressBar(y, h, 100);
    }
    printProgressBar(y, h, 100);
    for(int x = 0; x < w; ++x)
    {
      int curr = rand()%2;
      
      if(curr == prev)
      {
        count++;
      }
      else
      {
        if(count != 1)
          fprintf(f, "%d", count);
        
        if(prev != -1)
          fprintf(f, "%c", charval(prev));

        prev = curr;
        count = 1;
      }
    }

    if(prev == 1)
    {
      if(count != 1)
        fprintf(f, "%d", count);

      fprintf(f, "o");
    }

    if(y == h - 1)
      fprintf(f, "!");
    else
      fprintf(f, "$");
  }
  printProgressBar(h, h, 100);
  printf("\nFile made.\n");

  fclose(f);

  return 0;
}
