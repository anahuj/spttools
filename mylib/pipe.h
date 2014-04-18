
#ifndef PIPE_H
#define PIPE_H

void *pipe_new_p2c();
void *pipe_new_c2p();
void pipe_apply_atchild(void *);
void pipe_apply_atparent(void *);
int pipe_get_readfd(void *);
int pipe_get_writefd(void *);
/* void pipe_write(ty_pipe *,char *text); */

#endif
