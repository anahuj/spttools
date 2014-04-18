
#include <stdlib.h>
#include "gsl_taus.h"

typedef struct {
  char tausmem[24];
} ty_random;

void *random_new(int seed)
{
  ty_random *p;

  p = (ty_random *)malloc(sizeof(ty_random));
  taus_set(p->tausmem,(unsigned long)seed);
  return (void *)p;
}

void random_free(void *pp)
{
  ty_random *p;

  p = (ty_random *)pp;
  free(p);
}

int random_int(void *pp)
{
  ty_random *p;

  p = (ty_random *)pp;
  return (int)taus_get(p->tausmem);
}

float random_float(void *pp)
{
  ty_random *p;

  p = (ty_random *)pp;
  return (float)(0.9999999*taus_get_double(p->tausmem));
}

double random_double(void *pp)
{
  ty_random *p;

  p = (ty_random *)pp;
  return taus_get_double(p->tausmem);
}

