
#ifndef RANDOM_H
#define RANDOM_H

void *random_new(int);
void random_free(void *);
int random_int(void *);
float random_float(void *);
double random_double(void *);

// FIXME: We also need to reset the generator for a given seed
// if we want repeat something exactly the same at multiple times.

#endif
