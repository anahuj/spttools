
#ifndef BUFFER_H
#define BUFFER_H

void *buffer_new(unsigned char *, int);
void buffer_free(void *);
void buffer_set_offset(void *, int);
int buffer_find_string(void *, char *);
int buffer_find_string_back(void *, char *);
int buffer_find_char(void *, char);
int buffer_find_char_back(void *, char);
void buffer_skip_bytes(void *, int);
int buffer_getlen(void *);
int buffer_gettaillen(void *);
int buffer_getloc(void *);
void buffer_setloc(void *, int);
unsigned char *buffer_getbuf(void *);
unsigned char *buffer_getbuf_atloc(void *);
char buffer_read_char(void *);
unsigned char buffer_read_uchar(void *);
int buffer_read_int(void *);
int buffer_read_iint(void *);
int buffer_read_int_inv(void *);
int buffer_read_int_print(void *);
unsigned int buffer_read_uint(void *);
unsigned int buffer_read_iuint(void *);
int buffer_read_short(void *);
int buffer_read_ishort(void *);
int buffer_read_short_inv(void *);
int buffer_read_ushort(void *);
int buffer_read_iushort(void *);
int buffer_read_byte(void *);
int buffer_read_ubyte(void *);
float buffer_read_float(void *);
float buffer_read_ifloat(void *);
int buffer_eof(void *);
char *buffer_read_intstring(void *);
char *buffer_read_nstring(void *, int);
char *buffer_read_nullstring(void *);
char *buffer_read_string4(void *);
void buffer_read_string4_in(void *, char *);
char *buffer_read_bytes(void *, int);
void buffer_read_bytes_in(void *, int, char *);
char *buffer_read_line(void *);

#endif
