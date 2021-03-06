#ifndef STRINGS_H
#define STRINGS_H

#include "types.h"

/* Returns the length of the specified string */
int strlen(const char *);

/* Compares both strings and returns if they are equal */
boolean strcmp(char *, char *);

/* Compares both strings and returns if their first num characters are equal*/
boolean strncmp(char *, char *, int);

/* Copy the string from source to dest*/
char * strcpy(char *, const char *);

#endif
