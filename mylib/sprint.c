
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "sprint.h"

#define SPRINT_BLOCKSIZE 256

char *global_sprint_s = NULL;
int global_sprint_size = 0;
int global_sprint_end = 0;

// creates new string; uses malloc
char *sprint(char *format, ...)
{
  va_list ap;
  char *s;
  int k;

  global_sprint_end = 0; // sprint starts at beginning with no memory
  global_sprint_size = SPRINT_BLOCKSIZE;
  global_sprint_s = (char *)malloc(SPRINT_BLOCKSIZE*sizeof(char));
  if (global_sprint_s == NULL) return NULL;

  while (1) {
    va_start(ap,format);
    k = vsnprintf(&(global_sprint_s[global_sprint_end]),global_sprint_size,
		  format,ap);
    va_end (ap);

    if (k < global_sprint_size) {
      global_sprint_end += k;
      return global_sprint_s;
    }

    global_sprint_size += SPRINT_BLOCKSIZE;
    s = global_sprint_s; // keep old address for free() in the case of failure
    global_sprint_s = (char *)realloc(global_sprint_s,global_sprint_size);
    if (global_sprint_s == NULL) {
      free(s);
      return NULL;
    }
  }
}

// appends to string
char *sprinta(char *format, ...)
{
  va_list ap;
  char *s;
  int k;

  if (global_sprint_s == NULL) {
    global_sprint_end = 0;
    global_sprint_size = SPRINT_BLOCKSIZE;
    global_sprint_s = (char *)malloc(SPRINT_BLOCKSIZE*sizeof(char));
    if (global_sprint_s == NULL) return NULL;
  }

  while (1) {
    va_start(ap,format);
    k = vsnprintf(&(global_sprint_s[global_sprint_end]),global_sprint_size,
		  format,ap);
    va_end (ap);

    if (k < global_sprint_size) {
      global_sprint_end += k;
      return global_sprint_s;
    }

    global_sprint_size += SPRINT_BLOCKSIZE;
    s = global_sprint_s; // keep old address for free() in the case of failure
    global_sprint_s = (char *)realloc(global_sprint_s,global_sprint_size);
    if (global_sprint_s == NULL) {
      free(s);
      return NULL;
    }
  }
}

#if 0
// Example
int main(int ac, char **av)
{
  char *s;
  int k;

  k = 1;

  s = sprint("val = %i\n",k);
  s = sprinta("more\n");
  printf("%send\n",s);
  free(s);

  sprint("val = ");
  s = sprinta("%i\n",k);
  s = sprinta("more\n");
  printf("%send\n",s);
  free(s);

  return(1);
}
#endif
