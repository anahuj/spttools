
#ifndef FILESDIRS_H
#define FILESDIRS_H

int file_getsize_fd(int);
int file_getsize_path(char *);
int file_exists_name(char *);
int file2buffer(char *, unsigned char **, int *);
int buffer2file(unsigned char *, int, char *);
int buffer2stdout(unsigned char *, int);

void findfiles(char *, char ***, int *);

#endif
