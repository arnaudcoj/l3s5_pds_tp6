#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "makeargv.h"
#include <errno.h>
#include <getopt.h>

enum opts {OR=0, AND=1, CC=2, KILL=4};

int getopts(int argc, char **argv);
int m_do (int argc, char **argv, int opts);
int create_all_forks(int argc, char **argv);
int wait_proc(int nbproc, int opts);

int main (int argc, char **argv)
{
  int opts;
  opts = getopts(argc, argv);
  if(opts == 63)
    exit(EXIT_FAILURE);
  return m_do(argc, argv, opts);
}

int getopts(int argc, char **argv)
{
  int val;
  int tmp;
  /* on définit les options qu'on veut trouver*/
  struct option opts[5] = {
    {"and", no_argument, NULL, AND},
    {"or", no_argument, NULL, OR},
    {"cc", no_argument, NULL, CC},
    {"kill", no_argument, NULL, KILL},
    {0,0,0,0}
  };
  tmp = val = 0;
  /*tq il y a des options, on les ajoute à val*/
  while (tmp != -1)
    {
      val |= tmp;
      tmp = getopt_long(argc, argv, "", opts, NULL);
    }
  return val;
}


/**
   on lance les processus de argv en prenant en compte les options
*/
int m_do (int argc, char **argv, int opts)
{
  int nb_procs;

  /*
    on vérifie si on a assez d'arguments
  */
  if(argc <= 1)
    {
      printf("Pas assez d'arguments\n");
      exit(EXIT_FAILURE);
    }

  nb_procs = create_all_forks(argc, argv);

  return wait_proc(nb_procs, opts);
}


/**
   on crée le nombre de forks voulus
   pour chaque fils, on lance une commande
   @return le nombre de fils lancés
*/
int create_all_forks(int argc, char **argv)
{
  int i, nb_procs;
  pid_t pid;
  char **args;
  nb_procs = 0;
  pid = 1;
  args = NULL;
  for(i = 1; i < argc; i++)
    {
      /* si fils on sort de la boucle */
      if(pid == 0)
	break;

      if(argv[i][0] != '-')
	{
	  nb_procs++;
	  makeargv(argv[i], " ", &args);

	  switch(pid = fork())
	    {
	    case -1: /* erreur */
	      perror("erreur fork");
	      exit(EXIT_FAILURE);
	    case 0: /* fils : execute une commande */
	      execvp(args[0], args);
	      break;
	    default:
	      break;
	    }

	  freeargv(args);
	}
    }
  return nb_procs;
}

/**
   on attend la fin des commandes
   on stocke les valeurs de retour dans statusfinal en fonction
   de l'option --and (par défaut) ou --or
*/
int wait_proc(int nbproc, int opts)
{
  int i;
  int statusfinal;
  int status;
  wait(&status);
  statusfinal = WEXITSTATUS(status);
  for(i = 0; i < nbproc; i++)
    {
      wait(&status);
      if(opts & AND)
	statusfinal &= WEXITSTATUS(status);
      else
	statusfinal |= WEXITSTATUS(status);
    }

  return statusfinal;
}
