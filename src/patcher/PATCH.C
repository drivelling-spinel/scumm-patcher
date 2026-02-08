#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"


void patchScript200(const char * fname)
{
  FILE * f;
  const char * needle = "Heap Sta";
  long long i, j, pos, l;
  int c, pc = 0, L, ch;

  f = fopen(fname, "rb+");
  if(!f) 
  {
    perror("Failed to open file for patching");
    return;
  }

  fseek(f, 0, 2);
  l = ftell(f);
  fseek(f, pos = 0, 0);
  j = 0;
  L = strlen(needle);

  for(i = 0; i < 2 && pos < l ; i++, pos++) fgetc(f);
  
  while(pos < l)
  {
    c = fgetc(f);
    pos++;
    if(needle[j] == c) j++;
    else if(needle[0] == c) j = 1;
    else j = 0;
    if(j != L) continue;

    ch = j = i = 0;
    do 
    {
      fseek(f, pos - L - (ch + 1) * 2, 0);
      c = fgetc(f);
      if(c == 0xb7) ch += 1;
    } while(pos - L - (ch + 1) * 2 >= 0 && c == 0xb7);
    if(!ch) goto rewind;
    j = ch * 2;
    fseek(f, pos - L, 0);
    do{
      c = fgetc(f);
      if(!c) ch--;
      fseek(f, pos - L + i, 0);
      i++;
      fputc(0, f);
      fseek(f, pos - L + i, 0);
    } while((ch || c) && pos - L + i < l);
    fseek(f, pos - L - j, 0);
    fputc(0x73, f);
    fputc((i+j-3)&0xff, f);
    fputc(((i+j-3)>>8)&0xff, f);
    pos += i - L;
    j = 0;
    pc += 1;
rewind:
    fseek(f, pos, 0);
  }

  
  fclose(f);
  if(!pc)
  {    
    fprintf(stderr, "File not patched - incompatible game or patched already?");
    return;
  }
  sync();
}
