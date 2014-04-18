

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// Array of size N in which all entries 0 to N-1 are in use.
typedef struct {
  int size;
  int datasize;
  // Every array block has 1024 of data.
  int blockitems; // 1024
  int blocksize; // = datasize*blockitems
  int maxnumblocks; // 4096
  int numblocks;
  void ***blocks; // size is maxnumblocks
} ty_parray;


// XXXX This is for storing void * pointers only, not arbitrary
// XXXX data. See above for array_.

// Automatically expanding array. C++ would be better for
// implementing this transparently.
// Note: array_resize() is used to enlarge or shrink the array.
// Note: array_set() cannot point beyond the array size.
// Note: size cannot be 0.
void *parray_new(int size)
{
  ty_parray *p;
  int i;

  p = (ty_parray *)malloc(sizeof(ty_parray));
  p->size = size;
  p->blockitems = 1024;
  p->blocksize = p->blockitems; // void * is one item
  p->maxnumblocks = 4096;
  p->numblocks = (p->size - 1) / p->blockitems + 1; // (1024-1)/1024+1
  p->blocks = (void ***)malloc(p->maxnumblocks*sizeof(void **));
  for (i = 0; i < p->maxnumblocks; i++) {
    p->blocks[i] = NULL;
  }
  for (i = 0; i < p->numblocks; i++) {
    p->blocks[i] = (void **)malloc(p->blocksize*sizeof(void *));
  }

  return (void *)p;
}

void parray_free(void *pp)
{
  ty_parray *p;
  int i;

  p = pp;
  for (i = 0; i < p->numblocks; i++) {
    free(p->blocks[i]);
  }
  free(p->blocks);
  free(p);
}

void parray_set(void *pp, int n, void *ptr)
{
  ty_parray *p;
  int i,j;

  p = pp;
  i = n/p->blockitems;
  j = n % p->blockitems;
  // fprintf(stderr,"parray_set %i %i\n",i,j);
  p->blocks[i][j] = ptr;
}

void *parray_get(void *pp, int n)
{
  ty_parray *p;
  int i,j;
  void *q;

  p = pp;
  i = n/p->blockitems;
  j = n % p->blockitems;
  // fprintf(stderr,"step 111111\n");
  // fprintf(stderr,"parray_get %i %i\n",i,j);
  if (p->blocks[i] == NULL) {
    fprintf(stderr,"blocks[i] is NULL\n");
  }
  q = p->blocks[i][j];
  // fprintf(stderr,"step 111112\n");

  return q;
}

// Old data is valid after resize.
void parray_resize(void *pp, int newsize)
{
  ty_parray *p;
  int i;
  int newnumblocks;

  p = pp;
  newnumblocks = (newsize - 1) / p->blockitems + 1;
  if (newnumblocks < p->numblocks) {
    // Shrink.
    for (i = newnumblocks; i < p->numblocks; i++) {
      free(p->blocks[i]);
      p->blocks[i] = NULL;
    }
  } else if (newnumblocks > p->numblocks) {
    // Enlarge.
    if (newnumblocks >= p->maxnumblocks) {
      fprintf(stderr,"array too large, size %i, newnumblocks %i >= maxnumblocks %i\n",newsize,newnumblocks,p->maxnumblocks);
    } else {
      for (i = p->numblocks; i < newnumblocks; i++) {
	p->blocks[i] = (void **)malloc(p->blocksize*sizeof(void *));
      }
    }
  }
  p->size = newsize;
  p->numblocks = newnumblocks;
}


