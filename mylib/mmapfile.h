
#ifndef MMAPFILE_H
#define MMAPFILE_H

void *mmapfile_open_r(char *); // Map file read-only.
void *mmapfile_create(char *,int); // Create new file of the given size.
void mmapfile_close(void *);
unsigned char *mmapfile_getbuf(void *); // Get the address of the buffer.
int mmapfile_getsize(void *); // Get the size of the buffer.

#endif
