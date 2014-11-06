#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "makeargv.h"

int main (int argc, char **argv)
{
  int i;
  pid_t pid ;
  char *args[2];
  
  /*
    on vérifie si on a assez d'arguments
  */
  if(argc <= 1)
    {
      perror("do : Pas assez d'arguments");
      exit(EXIT_FAILURE);
    }

  /*
    on crée le nombre de forks voulus
  */
  for(i = 1; i < argc; i++)
    {
      args[0] = argv[i];
      args[1] = NULL;
      if(pid != 0)
	{
	  pid = fork();
	  switch(pid)
	    {
	    case -1: /* erreur */
	      perror("erreur fork");
	      exit(EXIT_FAILURE);
	    case 0: /* fils : execute une commande */
	      execvp(argv[i], args);
	      break;
	    default:
	      break;
	    }
	}
    }

  for(i = 1; i < argc; i++)
    wait(NULL);

  return EXIT_SUCCESS;
}

