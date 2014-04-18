
# Copyright 2006 Juhana Sadeharju

CC=gcc -O2 -Wall
CFLAGS= 
FLAGS=
LIBS= -lm -ljpeg -lpng
LD_LIBRARY_PATH=

OBJS_SP= mylib/image.o mylib/buffer.o mylib/filesdirs.o sptparser.o
OBJS_SC= sptcompiler.o
OBJS_SDCM= sptdrawcompositemap.o

all:: sptparser sptcompiler

sptparser: $(OBJS_SP)
	$(CC) $(FLAGS) -o sptparser $(OBJS_SP) $(LIBS)

sptcompiler: $(OBJS_SC)
	$(CC) $(FLAGS) -o sptcompiler $(OBJS_SC) $(LIBS)

sptdrawcompositemap: $(OBJS_SDCM)
	$(CC) $(FLAGS) -o sptdrawcompositemap $(OBJS_SDCM) $(LIBS)

clean:
	rm -f *.o

mrproper:
	rm -f *.o sptparser sptcompiler sptdrawcompositemap
