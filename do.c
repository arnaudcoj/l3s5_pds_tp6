#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "makeargv.h"
#include <errno.h>
#include <getopt.h>
#include <sys/types.h>
#include <signal.h>

enum opts {OR=0, AND=1, CC=2, KILL=4};

int getopts(int argc, char **argv);
int m_do (int argc, char **argv, int opts);
int create_all_forks(int argc, char **argv);
int wait_proc(int nbproc, int opts);
int wait_proc_cc(int nbproc, int opts);
void kill_proc(int remaining_proc);

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

  if(opts & CC)
    return wait_proc_cc(nb_procs, opts);
  else
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
  if(opts & CC)
    exit(EXIT_FAILURE);
  wait(&status);
  statusfinal = WEXITSTATUS(status);
  for(i = 1; i < nbproc; i++)
    {
      wait(&status);
      if(opts & AND)
	statusfinal &= WEXITSTATUS(status);
      else
	statusfinal |= WEXITSTATUS(status);
    }

  return statusfinal;
}


/**
   lancée quand l'option -cc est demandée
   on attend la fin des commandes exécutées.
   Dès qu’une des commandes retourne un succès pour l’option
   --or, ou retourne un échec pour l’option --and, on retourne
   cette valeur sans attendre les prochains processus.
*/
int wait_proc_cc(int nbproc, int opts)
{
  int i;
  int res;
  int status;
  if(!(opts & CC))
    exit(EXIT_FAILURE);
  for(i = 0; i < nbproc; i++)
    {
      wait(&status);
      res = WEXITSTATUS(status);
      if(((opts & AND) && res) || (!(opts & AND) && res == 0))
	{
	  if(opts & KILL)
	    kill_proc(nbproc - i);
	  return res;
	}
    }
  /*
    pas la peine de vérifier le dernier res.
    Si on arrive jusqu'ici, c'est que les précédents processus n'ont pas
    retourné la valeur court-circuit, le dernier processus définit donc
    le message de retour et n'a pas de processus en attente à tuer.
  */
  return res;
}

/**
   Tue tous les processus restants
   @param remaining_proc le nombre de processus à tuer
*/
void kill_proc(int remaining_proc)
{
  int i;
  for(i = 0; i < remaining_proc; i++)
    kill(0, SIGINT);
  return;
}
