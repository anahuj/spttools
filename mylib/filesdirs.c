
// 20070416  Parameter order of buffer2file() was changed.
// 20070416  The buffer is now unsigned char.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

int file_getsize_fd(int n)
{
  struct stat file_stats;

  if (fstat(n,&file_stats) == -1) return -1;
  return file_stats.st_size;
}

int file_getsize_path(char *s)
{
  struct stat file_stats;

  if (stat(s,&file_stats) == -1) return -1;
  return file_stats.st_size;
}

int file_exists_name(char *s)
{
  struct stat file_stats;

  if (stat(s,&file_stats) == -1) return 0;
  return 1;
}

// Read the whole file to a buffer.
int file2buffer(char *s, unsigned char **buf, int *len)
{
  int fd,l;
  char *b;

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

int buffer2file(unsigned char *buf, int len, char *s)
{
  int fd;

  if ((fd = open(s,O_WRONLY|O_CREAT|O_TRUNC,0600)) == -1) {
    fprintf(stderr,"buffer2file: could not open file %s\n",s);
    return 0;
  }
  if (write(fd,buf,len) != len) {
    fprintf(stderr,"buffer2file: could not write file %s\n",s);
    return 0;
  }
  close(fd);
  return 1;
}

int buffer2stdout(unsigned char *buf, int len)
{
  if (write(STDOUT_FILENO,buf,len) != len) {
    fprintf(stderr,"buffer2file: could not write to stdout\n");
    return 0;
  }
  return 1;
}

void findfiles(char *s, char ***files, int *numfiles)
{
  struct dirent **namelist;
  char **fs;
  int i,n,nfs;
  char *stmp;

  stmp = (char *)malloc(10000*sizeof(char));
  n = scandir(s,&namelist,NULL,alphasort);
  if (n == -1) {
    fprintf(stderr,"error in findfiles; returning empty list\n");
    fs = NULL;
    nfs = 0;
  } else {
    nfs = 0;
    fs = (char **)malloc(n*sizeof(char *));
    for (i = 0; i < n; i++) {
      if ((strcmp(namelist[i]->d_name,".") != 0) &&
	  (strcmp(namelist[i]->d_name,"..") != 0)) {
	sprintf(stmp,"%s/%s",s,namelist[i]->d_name);
      // fprintf(stderr,"file \"%s\"\n",stmp);
	fs[nfs] = strdup(stmp);
	nfs++;
      }
    }
  }
  free(stmp);

  *files = fs;
  *numfiles = nfs;
}

