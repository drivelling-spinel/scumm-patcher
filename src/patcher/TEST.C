#include <stdio.h>

void main(int argc, char *argv[])
{
  if(argc > 1) patchScript200(argv[1]);
  else printf("Usage: %s <file-to-patch>\n", argv[0]);
}
