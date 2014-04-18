
#include <stdio.h>
#include <sys/time.h>

int global_clock_seconds1;
int global_clock_useconds1;
int global_clock_seconds2;
int global_clock_useconds2;

void clock_begin()
{
  struct timeval t;

  gettimeofday(&t,NULL);

  global_clock_seconds1 = t.tv_sec;
  global_clock_useconds1 = t.tv_usec;
}

void clock_end()
{
  struct timeval t;

  gettimeofday(&t,NULL);

  global_clock_seconds2 = t.tv_sec;
  global_clock_useconds2 = t.tv_usec;
}

void clock_print()
{
  int s,us;

  s = global_clock_seconds2 - global_clock_seconds1;
  us = global_clock_useconds2 - global_clock_useconds1;
  if (us < 0) { us += 1000000; s--; }
  fprintf(stderr,"%i.%i\n",s,us);
}

float clock_get()
{
  int s,us;
  float r;
  struct timeval t;

  gettimeofday(&t,NULL);


  s = (int)(t.tv_sec) - global_clock_seconds1;
  us = (int)(t.tv_usec) - global_clock_useconds1;
  if (us < 0) { us += 1000000; s--; }
  r = (float)s + (float)us*(float)0.000001;
  // fprintf(stderr,"%f\n",r);
  return r;
}
