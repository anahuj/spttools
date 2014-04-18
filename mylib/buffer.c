
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <zlib.h>

typedef struct {
  unsigned char *buffer;
  int len;
  int n; // Current read location.
  int prevn; // Previous read location.
  int offset;
} ty_buffer;

// Read-only virtual file system.
typedef struct {
  unsigned char *buffer;
  int len;
  int n; // Current read location.
  int prevn; // Previous read location.
  // Virtual file part.
  FILE *fp;
  int fileloc; // Buffer's location in the file.
  int filen; // Current read location in the file: filen = fileloc + n.
  int filelen; // Length of the file.
} ty_virbuffer;


// For recursive parsing we windowize the original buffer.
ty_buffer *global_bufferstack[128];

void *buffer_new(unsigned char *buffer, int len)
{
  ty_buffer *p;

  p = (ty_buffer *)malloc(sizeof(ty_buffer));
  p->buffer = buffer;
  p->len = len;
  p->n = 0;
  p->prevn = -1;
  p->offset = 0;
  return (void *)p;
}

void buffer_free(void *pp)
{
  ty_buffer *p;

  p = pp;
  free(p);
}

void buffer_set_offset(void *pp, int offset)
{
  ty_buffer *p;

  p = pp;
  p->offset = offset;
}

ty_virbuffer *virbuffer_new(FILE *fp, unsigned char *buffer, int len)
{
  ty_virbuffer *p;

  p = (ty_virbuffer *)malloc(sizeof(ty_virbuffer));
  p->buffer = buffer;
  p->len = len;
  p->n = 0;
  p->prevn = -1;
  p->fp = fp;
  p->fileloc = 0;
  p->filen = p->n + p->fileloc;
  p->filelen = 0; // Length of the file.
  return p;
}

void virbuffer_update(ty_virbuffer *p)
{
  
}

int buffer_find_string(void *pp, char *s)
{
  ty_buffer *p;
  int i,j,k,slen;

  p = pp;
  slen = strlen(s);
  for(i = p->n; i < p->len-slen+1; i++) {
    k = 0;
    for(j = 0; j < slen; j++) {
      if (p->buffer[i+j] == s[j]) k++;
    }
    if (k == slen) {
      p->prevn = p->n;
      p->n = i;
      return i;
    }
  }
  return -1;
}

int buffer_find_string_back(void *pp, char *s)
{
  ty_buffer *p;
  int i,j,k,slen,start;

  p = pp;
  slen = strlen(s);
  start = p->n - slen + 1;
  if (start < 0) return -1;

  for(i = start; i >= 0; i--) {
    k = 0;
    for(j = 0; j < slen; j++) {
      if (p->buffer[i+j] == s[j]) k++;
    }
    if (k == slen) {
      p->prevn = p->n;
      p->n = i;
      return i;
    }
  }
  return -1;
}

int buffer_find_char(void *pp, char c)
{
  ty_buffer *p;
  int i;

  p = pp;
  for(i = p->n; i < p->len; i++) {
    if (p->buffer[i] == c) {
      p->prevn = p->n;
      p->n = i;
      return i;
    }
  }
  return -1;
}

int buffer_find_char_back(void *pp, char c)
{
  ty_buffer *p;
  int i;

  p = pp;
  for(i = p->n; i >= 0; i--) {
    if (p->buffer[i] == c) {
      p->prevn = p->n;
      p->n = i;
      return i;
    }
  }
  return -1;
}

void buffer_skip_bytes(void *pp, int n)
{
  ty_buffer *p;

  p = pp;
  p->n += n;
}

int buffer_getlen(void *pp)
{
  ty_buffer *p;

  p = pp;
  return p->len;
}

int buffer_gettaillen(void *pp)
{
  ty_buffer *p;

  p = pp;
  return p->len - p->n;
}

int buffer_getloc(void *pp)
{
  ty_buffer *p;

  p = pp;
  return p->offset + p->n;
}

void buffer_setloc(void *pp, int n)
{
  ty_buffer *p;

  p = pp;
  p->n = n - p->offset;
}

unsigned char *buffer_getbuf(void *pp)
{
  ty_buffer *p;

  p = pp;
  return p->buffer;
}

unsigned char *buffer_getbuf_atloc(void *pp)
{
  ty_buffer *p;

  p = pp;
  return &(p->buffer[p->n]);
}

void virbuffer_skip_bytes(ty_virbuffer *p, int n)
{
  p->filen += n;
  virbuffer_update(p);
}

int virbuffer_getn(ty_virbuffer *p)
{
  return p->filen;
}

void virbuffer_setn(ty_virbuffer *p, int n)
{
  p->filen = n;
  virbuffer_update(p);
}

char buffer_read_char(void *pp)
{
  ty_buffer *p;
  char c;

  p = pp;
  c = p->buffer[p->n];
  p->n++;

  return c;
}

char virbuffer_read_char(ty_virbuffer *p)
{
  char c;

  c = p->buffer[p->n];
  p->filen++;
  virbuffer_update(p);

  return c;
}

unsigned char buffer_read_uchar(void *pp)
{
  ty_buffer *p;
  unsigned char c;

  p = pp;
  c = p->buffer[p->n];
  p->n++;
  // fprintf(stderr,"uchar = %i\n",(int)c);

  return c;
}

unsigned char virbuffer_read_uchar(ty_virbuffer *p)
{
  unsigned char c;

  c = p->buffer[p->n];
  p->filen++;
  virbuffer_update(p);
  // fprintf(stderr,"uchar = %i\n",(int)c);

  return c;
}

#if 0
int buffer_read_int(ty_buffer *p)
{
  unsigned char a[4];

  if (p->n >= p->len + 4) {
    fprintf(stderr,"EOF approaching too soon\n");
    exit(-1);
  }
  a[0] = buffer_read_uchar(p);
  a[1] = buffer_read_uchar(p);
  a[2] = buffer_read_uchar(p);
  a[3] = buffer_read_uchar(p);

  return ((int *)a)[0];
}

int buffer_read_short(ty_buffer *p)
{
  ty_buffer *p;
  unsigned char a[4];

  a[0] = buffer_read_uchar(p);
  a[1] = buffer_read_uchar(p);
  a[2] = 0;
  a[3] = 0;

  return ((int *)a)[0];
}

int buffer_read_byte(ty_buffer *p)
{
  ty_buffer *p;
  unsigned char a[4];

  a[0] = buffer_read_uchar(p);
  a[1] = 0;
  a[2] = 0;
  a[3] = 0;

  return ((int *)a)[0];
}
#endif

int buffer_read_int(void *pp)
{
  ty_buffer *p;
  int n;

  p = pp;
  if (p->n > p->len - 4) { // >= p->len + 4 XXXX why?
    fprintf(stderr,"EOF approaching too soon\n");
    return 0;
  }
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[2] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[3] = buffer_read_uchar(p);

  return n;
}

int buffer_read_iint(void *pp)
{
  ty_buffer *p;
  int n;

  p = pp;
  if (p->n > p->len - 4) {
    fprintf(stderr,"EOF approaching too soon\n");
    return 0;
  }
  ((unsigned char *)(&n))[3] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[2] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);

  return n;
}

int buffer_read_int_inv(void *pp)
{
  ty_buffer *p;
  int n;

  p = pp;
  if (p->n > p->len - 4) {
    fprintf(stderr,"EOF approaching too soon\n");
    return 0;
  }
  ((unsigned char *)(&n))[3] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[2] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);

  return n;
}

int buffer_read_int_print(void *pp)
{
  ty_buffer *p;
  int n;

  p = pp;
  if (p->n > p->len - 4) {
    fprintf(stderr,"EOF approaching too soon\n");
    return 0;
  }
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[2] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[3] = buffer_read_uchar(p);

  printf("int = %i\n",n);
  return n;
}

unsigned int buffer_read_uint(void *pp)
{
  ty_buffer *p;
  unsigned int n;

  p = pp;
  if (p->n > p->len - 4) {
    fprintf(stderr,"EOF approaching too soon\n");
    return 0;
  }
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[2] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[3] = buffer_read_uchar(p);

  return n;
}

unsigned int buffer_read_iuint(void *pp)
{
  ty_buffer *p;
  unsigned int n;

  p = pp;
  if (p->n > p->len - 4) {
    fprintf(stderr,"EOF approaching too soon\n");
    return 0;
  }
  ((unsigned char *)(&n))[3] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[2] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);

  return n;
}

#if 0
// XXXX This is actually ushort. Signed short needs [3] = uc, [3] >> 24
int buffer_read_short(void *pp)
{
  ty_buffer *p;
  int n;

  p = pp;
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[2] = 0;
  ((unsigned char *)(&n))[3] = 0;

  // fprintf(stderr,"short = %i\n",n);
  // printf("short = %i\n",n);
  return n;
}
#endif

int buffer_read_short(void *pp)
{
  ty_buffer *p;
  short n;

  p = pp;
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[1] = buffer_read_uchar(p);

  // fprintf(stderr,"short = %i\n",n);
  // printf("short = %i\n",n);
  return (int)n;
}

int buffer_read_ishort(void *pp)
{
  ty_buffer *p;
  short n;

  p = pp;
  ((unsigned char *)(&n))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);

  // fprintf(stderr,"short = %i\n",n);
  // printf("short = %i\n",n);
  return (int)n;
}

int buffer_read_short_inv(void *pp)
{
  ty_buffer *p;
  short n;

  p = pp;
  ((unsigned char *)(&n))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);

  // fprintf(stderr,"short = %i\n",n);
  // printf("short = %i\n",n);
  return (int)n;
}

int buffer_read_ushort(void *pp)
{
  ty_buffer *p;
  int n;

  p = pp;
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[2] = 0;
  ((unsigned char *)(&n))[3] = 0;

  // fprintf(stderr,"short = %i\n",n);
  // printf("short = %i\n",n);
  return n;
}

int buffer_read_iushort(void *pp)
{
  ty_buffer *p;
  int n;

  p = pp;
  ((unsigned char *)(&n))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[2] = 0;
  ((unsigned char *)(&n))[3] = 0;

  // fprintf(stderr,"short = %i\n",n);
  // printf("short = %i\n",n);
  return n;
}

#if 0
// XXXX This is actually ubyte. Signed byte needs [3] = b, [3] >> 24
// XXXX 20080211 Would the above work?
int buffer_read_byte(void *pp)
{
  ty_buffer *p;
  int n;

  p = pp;
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[1] = 0;
  ((unsigned char *)(&n))[2] = 0;
  ((unsigned char *)(&n))[3] = 0;

  // fprintf(stderr,"byte = %i\n",n);
  // printf("byte = %i\n",n);
  return n;
}
#endif

// XXXX 20080211 version
int buffer_read_byte(void *pp)
{
  ty_buffer *p;
  int n;
  char c;

  p = pp;
  c = (char)buffer_read_uchar(p);
  n = c;

  // fprintf(stderr,"byte = %i\n",n);
  // printf("byte = %i\n",n);
  return n;
}

int buffer_read_ubyte(void *pp)
{
  ty_buffer *p;
  int n;

  p = pp;
  ((unsigned char *)(&n))[0] = buffer_read_uchar(p);
  ((unsigned char *)(&n))[1] = 0;
  ((unsigned char *)(&n))[2] = 0;
  ((unsigned char *)(&n))[3] = 0;

  // fprintf(stderr,"byte = %i\n",n);
  // printf("byte = %i\n",n);
  return n;
}


#if 0
// Does not work. Why?
float buffer_read_float(ty_buffer *p)
{
  unsigned char a[4];
  float *fp;
  float f;

  a[0] = buffer_read_uchar(p);
  a[1] = buffer_read_uchar(p);
  a[2] = buffer_read_uchar(p);
  a[3] = buffer_read_uchar(p);
  // fprintf(stderr,"a[] = %i, %i, %i, %i\n",(int)a[0],(int)a[1],(int)a[2],(int)a[3]);

  fp = (float *)(&(a[0]));
  f = fp[0];
  // fprintf(stderr,"float = %.40f\n",f);
  return f;
}
#endif


// How to fix the float accuracy problem?
// Now we need to print too many decimals.
float buffer_read_float(void *pp)
{
  ty_buffer *p;
  float f;

  p = pp;
  ((unsigned char *)(&f))[0] = buffer_read_uchar(p);
  ((unsigned char *)(&f))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&f))[2] = buffer_read_uchar(p);
  ((unsigned char *)(&f))[3] = buffer_read_uchar(p);

  // fprintf(stderr,"float = %f\n",f);
  // printf("float = %.16f\n",f);
  return f;
}

float buffer_read_ifloat(void *pp)
{
  ty_buffer *p;
  float f;

  p = pp;
  ((unsigned char *)(&f))[3] = buffer_read_uchar(p);
  ((unsigned char *)(&f))[2] = buffer_read_uchar(p);
  ((unsigned char *)(&f))[1] = buffer_read_uchar(p);
  ((unsigned char *)(&f))[0] = buffer_read_uchar(p);

  // fprintf(stderr,"float = %f\n",f);
  // printf("float = %.16f\n",f);
  return f;
}

// 20080608 Returning -1 for NULL buffer works like a sentinel
// when we have a stack of buffers which runs out in while loop.
int buffer_eof(void *pp)
{
  ty_buffer *p;

  if (pp == NULL) return -1;
  p = pp;
  if (p->n >= p->len) return 1;
  return 0;
}

// char *extractstring(ty_buffer *p)
char *buffer_read_intstring(void *pp)
{
  ty_buffer *p;
  int i,n;
  char *s;

  p = pp;
  n = buffer_read_int(p);
  // fprintf(stderr,"string len = %i\n",n);

  s = (char *)malloc((n+1)*sizeof(char));
  for (i = 0; i < n; i++) {
    s[i] = buffer_read_char(p);
  }
  s[n] = '\0';

  // fprintf(stderr,"string = %s\n",s);
  // printf("string = \"%s\"\n",s);
  return s;
}

char *buffer_read_nstring(void *pp, int n)
{
  ty_buffer *p;
  int i;
  char *s;

  p = pp;
  s = (char *)malloc((n+1)*sizeof(char));
  for (i = 0; i < n; i++) {
    s[i] = buffer_read_char(p);
  }
  s[n] = '\0';

  return s;
}

char *buffer_read_nullstring(void *pp)
{
  ty_buffer *p;
  int i,n;
  char *s;
  int loc;

  p = pp;
  loc = buffer_getloc(p);
  while (buffer_read_char(p) != 0);
  n = buffer_getloc(p) - loc;
  buffer_setloc(p,loc);
  // s = (char *)malloc((n+1)*sizeof(char));
  s = (char *)malloc(n*sizeof(char));
  for (i = 0; i < n; i++) {
    s[i] = buffer_read_char(p);
  }
  // s[n] = '\0'; // This is second null.

  return s;
}

// char *extractstring4(ty_buffer *p)
char *buffer_read_string4(void *pp)
{
  ty_buffer *p;
  int i;
  char *s;

  p = pp;
  s = (char *)malloc(5*sizeof(char));
  for (i = 0; i < 4; i++) {
    s[i] = buffer_read_char(p);
  }
  s[4] = '\0';

  return s;
}

void buffer_read_string4_in(void *pp, char *s)
{
  ty_buffer *p;
  int i;

  p = pp;
  for (i = 0; i < 4; i++) {
    s[i] = buffer_read_char(p);
  }
  s[4] = '\0';
}

char *buffer_read_bytes(void *pp, int n)
{
  ty_buffer *p;
  int i;
  char *s;

  p = pp;
  s = (char *)malloc(n*sizeof(char));
  for (i = 0; i < n; i++) {
    s[i] = buffer_read_byte(p);
  }

  return s;
}

void buffer_read_bytes_in(void *pp, int n, char *s)
{
  ty_buffer *p;
  int i;

  p = pp;
  for (i = 0; i < n; i++) {
    s[i] = buffer_read_byte(p);
  }
}

char *buffer_read_line(void *b)
{
  int k1,k2,loc,n;
  char *s;

  loc = buffer_getloc(b);
  k1 = buffer_find_char(b,'\n');
  if (k1 == -1) {
    fprintf(stderr,"No line\n");
    s = strdup("");
    return s;
  }
  if (k1 == loc) {
    s = strdup("");
    buffer_setloc(b,k1+1);
    return s;
  }
  buffer_setloc(b,k1-1);
  n = buffer_read_char(b);
  if (n == '\r') {
    k2 = k1 - 1;
  } else {
    k2 = k1;
  }
  buffer_setloc(b,loc);
  s = buffer_read_nstring(b,k2-loc);
  buffer_setloc(b,k1+1);
  return s;
}

