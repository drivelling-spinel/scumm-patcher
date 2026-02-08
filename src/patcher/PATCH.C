#include "stdlib.h"
#include "stdio.h"
#include "string.h"

void patchScript200(const char * fname)
{
  FILE * f;
  const char * needle = "Heap Sta";
  int i, j;
  char c;
  long long pos = 0;

  f = fopen(fname, "r+");
  fseek(f, pos, 0);

  
  while(1)
  {
    break;
  }

  
  fclose(f);
  sync();
}
