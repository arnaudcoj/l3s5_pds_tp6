#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "makeargv.h"
#include <errno.h>
#include <getopt.h>

enum opts {OR=0, AND=1, CC=2, KILL=4};

int getargs(int argc, char **argv);
int m_do (int argc, char **argv, int opts);
void testmakeargv(int argc, char **argv);

int main (int argc, char **argv)
{
  int opts;
  opts = getargs(argc, argv);
  /* testmakeargv(argc, argv);*/
  if(opts == 63)
    exit(EXIT_FAILURE);
  printf("%d\n", opts);
  return 0;
  /*  return m_do(argc, argv, opts);*/
}

int getargs(int argc, char **argv)
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

void testmakeargv(int argc, char **argv)
{
  char **lol;
  int nbt, i, j;
  lol = NULL;
  for(i = 0; i < argc; i++)
    {
      nbt = makeargv(argv[i], " ", &lol);
      for(j = 0; j < nbt; j++)
	printf("%d:%s ", j, lol[j]);
      printf("\n");
      freeargv(lol);
    }
  return;
}

int m_do (int argc, char **argv, int opts)
{
  int i;
  pid_t pid ;
  char *args[2];
  int statusfinal;
  int status;
  i = 1;
  pid = 1;

  /*
    on vérifie si on a assez d'arguments
  */
  if(argc <= 1)
    {
      printf("Pas assez d'arguments\n");
      exit(EXIT_FAILURE);
    }

  /*
    on crée le nombre de forks voulus
    pour chaque fils, on lance une commande
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
	      printf("exec : %s\n", args[0]);
	      execvp(argv[i], args);
	      break;
	    default:
	      break;
	    }
	}
    }

  /*
    on attend la fin des commandes
    on stocke les valeurs de retour dans statusfinal en fonction
     de l'option --and (par défaut) ou --or
  */
  wait(&status);
  statusfinal = WEXITSTATUS(status);
  for(i = 2; i < argc; i++)
    {
      wait(&status);
      if(opts & AND)
	statusfinal &= WEXITSTATUS(status);
      else
	statusfinal |= WEXITSTATUS(status);
    }

  return statusfinal;
}
