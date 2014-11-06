#ifndef MAKEARGV_H

#define MAKEARGV_H
int makeargv(const char *s, const char *delimiters, char ***argvp);
void freeargv(char **argv);

#endif
