
/*

mmapfile -- mmap(2) based virtual file and shared memory system

The virtual file part means that the files can be accessed as
if they were located in the memory, e.g., "buffer[n] = value;".

The shared memory part means that the memory mapped files can
be used as a shared memory after fork(2).

 -*-

How to map an existing file for read and write?
 open(,O_RDWR);

How to enlarge the file with this system?
Existing file can be grown larger before mapping:
 open(); seek_end(); write(); mmap();
But not after? Realloc of the buffer?
 open(); mmap(); munmap(); seek_end(); write(); mmap();


Check to end of this file for stuff I have used earlier.
It could be better that we have separate routines to
open the files. mmap routines would see only the FDs and
file lengths.

 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#include "filesdirs.h"

typedef struct {
  int size;
  caddr_t mmapaddr;
} ty_mmapfile;

// Read-only.
void *mmapfile_open_r(char *s)
{
  ty_mmapfile *p;
  int fd;

  p = (ty_mmapfile *)malloc(sizeof(ty_mmapfile));
  if (p == NULL) return NULL;

  p->size = file_getsize_path(s);
  fd = open(s,O_RDONLY);
  p->mmapaddr = mmap(0,p->size,PROT_READ,MAP_PRIVATE,fd,0);
  if (p->mmapaddr == (caddr_t) -1) {
    fprintf(stderr,"mmap error, %s\n",s);
    close(fd);
    return(NULL);
  }
  close(fd);
  return (void *)p;
}

void *mmapfile_create(char *s, int size)
{
  ty_mmapfile *p;
  int fd;

  p = (ty_mmapfile *)malloc(sizeof(ty_mmapfile));
  p->size = size;
  fd = open(s,O_RDWR|O_CREAT|O_TRUNC,0600);
  (void)lseek(fd,p->size,0);
  write(fd,&fd,1);
  p->mmapaddr = mmap(0,p->size,PROT_WRITE,MAP_PRIVATE,fd,0);
  if (p->mmapaddr == (caddr_t) -1) {
    fprintf(stderr,"mmap error, %s\n",s);
    close(fd);
    return(NULL);
  }
  close(fd);
  return (void *)p;
}

void mmapfile_close(void *pp)
{
  ty_mmapfile *p;

  p = (ty_mmapfile *)pp;
  (void)munmap(p->mmapaddr,p->size);
}

unsigned char *mmapfile_getbuf(void *pp)
{
  ty_mmapfile *p;

  p = (ty_mmapfile *)pp;
  return (unsigned char *)p->mmapaddr;
}

int mmapfile_getsize(void *pp)
{
  ty_mmapfile *p;

  p = (ty_mmapfile *)pp;
  return p->size;
}





#if 0
ty_mmapfile *file_openmmap_part(char *s, int offset, int len)
{
}
#endif


#if 0
int main(int ac, char **av)
{
  ty_mmapfile *f1,*f2;
  int i,j,n,len;
  unsigned char *buf1,*buf2;
  int bsize1,bsize2;

  f1 = mmapfile_open(av[1]);
  n = atoi(av[2]);
  len = atoi(av[3]);
  f2 = mmapfile_open(av[4]);

  buf1 = mmapfile_getbuf(f1);
  bsize1 = mmapfile_getsize(f1);
  buf2 = mmapfile_getbuf(f2);
  bsize2 = mmapfile_getsize(f2);

  for(i = 0; i < bsize2-len+1; i++) {
    j = 0;
    while ((buf1[n+j] == buf2[i+j]) && (j < len)) {
      j++;
    }
    if (j == len) {
      printf("%i\n",i);
    }
  }

}

#endif

#if 0

#define C_MMAP_FILE 0

typedef struct {
  int type;
  int kind;  /* 0 = file mapped */
  char *filename;
  int fd;
  int filesize;
  unsigned char *mmapaddr;
} ty_mmap;

ty_mmap *mmap_file_rdonly(char *s)
{
  ty_mmap *p;

  p = (ty_mmap *)malloc(sizeof(ty_mmap));
  p->kind = C_MMAP_FILE;
  p->filename = strdup(s);
  p->fd = open(p->filename,O_RDONLY);
  p->filesize = file_getsize_fd(p->fd);
  p->mmapaddr = mmap(0,p->filesize,PROT_READ,MAP_SHARED,p->fd,0);
  return(p);
}

void mmap_free(ty_mmap *p)
{
  (void)munmap(p->mmapaddr,p->filesize);
  if (p->kind == C_MMAP_FILE) {
    free(p->filename);
    close(p->fd);
  }
  free(p);
}

unsigned char *mmap_getaddr(ty_mmap *p)
{
  return(p->mmapaddr);
}

int mmap_getlen(ty_mmap *p)
{
  return(p->filesize);
}

#endif


