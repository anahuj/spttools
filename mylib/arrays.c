
#include <stdlib.h>

char *array_char(int n)
{
  char *p;

  p = (char *)malloc(n*sizeof(char));
  return p;
}

short *array_short(int n)
{
  short *p;

  p = (short *)malloc(n*sizeof(short));
  return p;
}

int *array_int(int n)
{
  int *p;

  p = (int *)malloc(n*sizeof(int));
  return p;
}

float *array_float(int n)
{
  float *p;

  p = (float *)malloc(n*sizeof(float));
  return p;
}

double *array_double(int n)
{
  double *p;

  p = (double *)malloc(n*sizeof(double));
  return p;
}
