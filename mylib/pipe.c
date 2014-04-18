
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



typedef struct {
  int type;
  int fd[2];
  int dir;
} ty_pipe;


#define C_PIPE_P2C 1
#define C_PIPE_C2P 2

/* Create pipe for sending data from parent to child */
void *pipe_new_p2c()
{
  ty_pipe *p;
  int tmpfd[2];

  if (pipe(tmpfd) != 0) {
    fprintf(stderr,"could not create a pipe\n");
    return NULL;
  }
  p = (ty_pipe *)malloc(sizeof(ty_pipe));
  p->fd[0] = tmpfd[0];
  p->fd[1] = tmpfd[1];
  p->dir = C_PIPE_P2C;

  return p;
}

/* Create pipe for sending data from child to parent */
void *pipe_new_c2p()
{
  ty_pipe *p;
  int tmpfd[2];

  if (pipe(tmpfd) != 0) {
    fprintf(stderr,"could not create a pipe\n");
    return NULL;
  }
  p = (ty_pipe *)malloc(sizeof(ty_pipe));
  p->fd[0] = tmpfd[0];
  p->fd[1] = tmpfd[1];
  p->dir = C_PIPE_C2P;

  return p;
}

void pipe_apply_atchild(void *pp)
{
  ty_pipe *p;

  p = pp;
  if (p->dir == C_PIPE_P2C) {
    /* child does not need the write head */
    close(p->fd[1]);
  } else {
    /* child does not need the read head */
    close(p->fd[0]);
  }
}

void pipe_apply_atparent(void *pp)
{
  ty_pipe *p;

  p = pp;
  if (p->dir == C_PIPE_P2C) {
    /* parent does not need the read head */
    close(p->fd[0]);
  } else {
    /* parent does not need the write head */
    close(p->fd[1]);
  }
}

int pipe_get_readfd(void *pp)
{
  ty_pipe *p;

  p = pp;
  return p->fd[0];
}

int pipe_get_writefd(void *pp)
{
  ty_pipe *p;

  p = pp;
  return p->fd[1];
}

