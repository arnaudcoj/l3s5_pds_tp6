#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char **argv)
{
  char **args;
  args = &argv[1];
  if(argc > 1)
    return execvp(argv[1], args);
  return EXIT_FAILURE;
}
