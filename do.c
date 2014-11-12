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
void create_all_forks(int argc, char **argv, pid_t *processes);
int wait_proc(int opts, pid_t *processes);
int wait_proc_cc(int opts, pid_t *processes);
void kill_proc(pid_t *processes);

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
  pid_t *processes;
  int res;
  processes = malloc(sizeof(pid_t)*(argc+1));
  processes[argc] = 0;  

  /*
    on vérifie si on a assez d'arguments
  */
  if(argc <= 1)
    {
      printf("Pas assez d'arguments\n");
      exit(EXIT_FAILURE);
    }

  create_all_forks(argc, argv, processes);

  if(opts & CC)
    res = wait_proc_cc(opts, processes);
  else
    res = wait_proc(opts, processes);
  free(processes);
  return res;
}


/**
   on crée le nombre de forks voulus
   pour chaque fils, on lance une commande
   les pids des fils sont stockés dans processes
   on les stocke dans l'ordre décroissant car getopts 'range' les paramètres
   au début de argv, de cette façon, on met des 0 à la fin du tableau pour
   wait/kill les processus voulus et savoir quand il n'y en a plus.
*/
void create_all_forks(int argc, char **argv, pid_t *processes)
{
  int i;
  pid_t pid;
  char **args;
  pid = 1;
  args = NULL;
  for(i = 1; i < argc; i++)
    {
      /* si fils on sort de la boucle */
      if(pid == 0)
	break;

      if(argv[i][0] != '-')
	{
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
	      processes[argc - i -1] = pid; 
	      break;
	    }

	  freeargv(args);
	}
      else
	  processes[argc - i - 1] = 0;
    }
  return;
}

/**
   on attend la fin des commandes
   on stocke les valeurs de retour dans statusfinal en fonction
   de l'option --and (par défaut) ou --or
*/
int wait_proc(int opts, pid_t *processes)
{
  int i;
  int statusfinal;
  int status;
  i = 0;
  if(opts & CC)
    exit(EXIT_FAILURE);
  wait(&status);
  statusfinal = WEXITSTATUS(status);
  while(processes[i])
    {
      wait(&status);
      if(opts & AND)
	statusfinal &= WEXITSTATUS(status);
      else
	statusfinal |= WEXITSTATUS(status);
      i++;
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
int wait_proc_cc(int opts, pid_t *processes)
{
  int i;
  int res;
  int status;
  i = 0;
  if(!(opts & CC))
    exit(EXIT_FAILURE);
  while(processes[i])
    {
      wait(&status);
      res = WEXITSTATUS(status);
      if(((opts & AND) && res) || (!(opts & AND) && res == 0))
	{
	  if(opts & KILL)
	    kill_proc(processes);
	  return res;
	}
      i++;
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
void kill_proc(pid_t *processes)
{
  int i;
  i = 0;
  while(processes[i])
    kill(processes[i++], SIGINT);
  return;
}
