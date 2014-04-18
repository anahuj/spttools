
#ifndef PARRAY_H
#define PARRAY_H

void *parray_new(int);
void parray_free(void *);
void parray_set(void *, int, void *);
void *parray_get(void *, int);
void parray_resize(void *, int);

#endif
