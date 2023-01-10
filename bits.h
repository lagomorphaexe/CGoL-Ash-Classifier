#include <stdio.h>

long pown(long base, int power)
{
  long out = 1;
  int binCounter = 0;

  while(power > 0)
  {
    if(power & (1 << binCounter))
    {
      power -= 1 << binCounter;
      out *= base;
    }
    base *= base;
    binCounter++;
  }

  return out;
}

int ilog(long n, int base)
{
    int out = 1;
    int c = 0;
    while(out <= n)
    {
        out *= base;
        c++; 
    }
    return c - 1;
}

void printProgressBar(int cur, int max, int barLength)
{
  printf("[");

  int barFill = cur * barLength / max;
  int fracFill = cur * barLength * 4 / max - barFill * 4;

  for(int i = 0; i < barFill; ++i)
    printf("█");
  if(barLength - barFill >= 1)
  {
    switch(fracFill)
    {
      case 0:
        printf(" ");
        break;
      case 1:
        printf("░");
        break;
      case 2:
        printf("▒");
        break;
      case 3:
        printf("▓");
        break;
    }
  }
  for(int i = 0; i < barLength - barFill - 1; ++i)
    printf(" ");

  printf("] %d of %d (%f%%)    \r", cur, max, (10000*cur/max)/(float)100);
}