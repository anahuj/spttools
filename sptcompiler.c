
// Copyright 2006 Juhana Sadeharju

// sptparser: writes the spt file to an editable file.
// sptcompiler: writes the editable file to the spt file.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


typedef struct {
  unsigned char *buffer;
  int len;
  int n; // Current read location.
  int prevn; // Previous read location.
} ty_buffer;

int file_getsize_fd(int n)
{
  struct stat file_stats;

  if (fstat(n,&file_stats) == -1) return -1;
  return file_stats.st_size;
}

// Read the whole file to a buffer.
int file2buffer(char *s, unsigned char **buf, int *len)
{
  int fd,l;
  unsigned char *b;

  *buf = NULL;
  *len = -1;

  if ((fd = open(s,O_RDONLY)) == -1) {
    fprintf(stderr,"file2buffer: could not open file %s\n",s);
    return 0;
  }
  l = file_getsize_fd(fd);
  b = (unsigned char *)malloc((l+10)*sizeof(unsigned char));
  if (read(fd,b,l) != l) {
    fprintf(stderr,"file2buffer: could not read file %s\n",s);
    free(b);
    return 0;
  }
  close(fd);
  b[l] = '\0';
  *len = l;
  *buf = b;
  return 1;
}

ty_buffer *buffer_new(unsigned char *buffer, int len)
{
  ty_buffer *p;

  p = (ty_buffer *)malloc(sizeof(ty_buffer));
  p->buffer = buffer;
  p->len = len;
  p->n = 0;
  p->prevn = -1;
  return p;
}

unsigned char buffer_read_uchar(ty_buffer *p)
{
  unsigned char c;

  c = p->buffer[p->n];
  p->n++;
  // fprintf(stderr,"uchar = %i\n",(int)c);

  return c;
}

// Is p->len-1 wrong? It is needed because scanning stops at p->len-1.
int buffer_eof2(ty_buffer *p)
{
  if (p->n >= p->len-1) return 1;
  return 0;
}

void buffer_scan_int(ty_buffer *p, int *n)
{
  int count;

  count = 0;
  sscanf(&(p->buffer[p->n]),"%i%n",n,&count);
  p->n += count;
  // fprintf(stderr,"p->n = %i, len = %i\n",p->n,p->len);
}

void buffer_scan_float(ty_buffer *p, float *f)
{
  int count;

  count = 0;
  sscanf(&(p->buffer[p->n]),"%f%n",f,&count);
  p->n += count;
  // fprintf(stderr,"p->n = %i, len = %i\n",p->n,p->len);
}

void buffer_scan_tokenstring(ty_buffer *p, char *s)
{
  int count;

  count = 0;
  sscanf(&(p->buffer[p->n]),"%s =%n",s,&count);
  p->n += count;
  // fprintf(stderr,"p->n = %i, len = %i\n",p->n,p->len);
}

unsigned char tmpus[20000];

int main(int ac, char **av)
{
  unsigned char *buffer;
  int len,n;
  ty_buffer *b;
  char *s;
  float f;
  unsigned char uc;
  int fd;

  if (ac < 2) {
    fprintf(stderr,"Usage: sptcompiler <file>\n");
    exit(-1);
  }

  file2buffer(av[1],&buffer,&len);

  b = buffer_new(buffer,len);

  s = (char *)malloc(10000*sizeof(char));

  fd = STDOUT_FILENO;
  while (buffer_eof2(b) != 1) {
    buffer_scan_tokenstring(b,s);
    if (strcmp(s,"section") == 0) {
      buffer_scan_int(b,&n);
      write(fd,&n,4);
    } else if (strcmp(s,"int") == 0) {
      buffer_scan_int(b,&n);
      write(fd,&n,4);
    } else if (strcmp(s,"float") == 0) {
      buffer_scan_float(b,&f);
      write(fd,&f,4);
    } else if (strcmp(s,"short") == 0) {
      buffer_scan_int(b,&n);
      write(fd,&n,2);
    } else if (strcmp(s,"byte") == 0) {
      buffer_scan_int(b,&n);
      write(fd,&n,1);
    } else if (strcmp(s,"string") == 0) {
      (void)buffer_read_uchar(b); // space
      (void)buffer_read_uchar(b); // quote
      n = 0;
      while ((uc = buffer_read_uchar(b)) != '"') {
	tmpus[n] = uc;
	n++;
      }
      write(fd,&n,4);
      write(fd,tmpus,n);
    } else if (strcmp(s,"desc") == 0) {
      (void)buffer_read_uchar(b); // space
      (void)buffer_read_uchar(b); // quote
      n = 0;
      while ((uc = buffer_read_uchar(b)) != '"') {
	n++;
      }
    } else {
      fprintf(stderr,"Wrong token \"%s\"\n",s);
      exit(-1);
    }
  }
  close(fd);

  return 1;
}
