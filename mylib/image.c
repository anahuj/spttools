
// Todo: Read ppm.gz images by reading to memory and then uncompressing;
// the popen with zcat is not fast enough.

// image_composite(op,srcimage1,srcimage2,destimage)

// Optimize image operations. See image_scale_zoom() for example.

// Reading the header of PPM/PGM/PBM/PNM files is puzzling.
// sscanf() fails badly.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <jpeglib.h>
#include <jerror.h>
#include <png.h>

#define C_IMAGE_RGB 0
#define C_IMAGE_G 1
#define C_IMAGE_RGBA 2
#define C_IMAGE_FRGB 3
#define C_IMAGE_FZ 4

// XXXX used in image_get_type()
char *global_image_type_rgb = "RGB";
char *global_image_type_g = "G";
char *global_image_type_rgba = "RGBA";

typedef struct {
  char type[8];
  int itype;
  int xsize;
  int ysize;
  unsigned char *image;
  float *fimage;
} ty_image;

typedef struct {
  char type[8];
  int itype;
  int xsize;
  int ysize;
  float *image;
} ty_floatimage;

void *image_convert_to_rgb(void *);

void image_split_string(char *s, char ***psa, int *psn)
{
  int i,n;
  char **sa;
  int sn;
  int k,loc;

  // Count delims.
  n = 0;
  for (i = 0; i < strlen(s); i++) {
    if (s[i] == ' ') n++;
  }

  // Split.
  sn = n+1;
  sa = (char **)malloc(sn*sizeof(char *));
  k = 0;
  loc = 0;
  for (i = 0; i < strlen(s); i++) {
    if (s[i] == ' ') {
      sa[k] = (char *)malloc((i-loc+1)*sizeof(char));
      strncpy(sa[k],&(s[loc]),i-loc);
      sa[k][i-loc] = '\0';
      k++;
      loc = i;
    }
  }
  sa[k] = (char *)malloc((strlen(s)-loc+1)*sizeof(char));
  strncpy(sa[k],&(s[loc]),strlen(s)-loc);
  sa[k][strlen(s)-loc] = '\0';

  *psa = sa;
  *psn = sn;
}



void *image_new(char *type, int xsize, int ysize)
{
  ty_image *p;

  p = (ty_image *)malloc(sizeof(ty_image));
  p->image = NULL;
  p->fimage = NULL;
  if (strcmp(type,"rgb") == 0) {
    sprintf(p->type,"rgb");
    p->itype = C_IMAGE_RGB;
    p->xsize = xsize;
    p->ysize = ysize;
    p->image = (unsigned char *)malloc(3*xsize*ysize*sizeof(unsigned char));
  } else if (strcmp(type,"rgba") == 0) {
    sprintf(p->type,"rgba");
    p->itype = C_IMAGE_RGBA;
    p->xsize = xsize;
    p->ysize = ysize;
    p->image = (unsigned char *)malloc(4*xsize*ysize*sizeof(unsigned char));
  } else if (strcmp(type,"g") == 0) {
    sprintf(p->type,"g");
    p->itype = C_IMAGE_G;
    p->xsize = xsize;
    p->ysize = ysize;
    p->image = (unsigned char *)malloc(xsize*ysize*sizeof(unsigned char));
  } else if (strcmp(type,"frgb") == 0) {
    sprintf(p->type,"frgb");
    p->itype = C_IMAGE_FRGB;
    p->xsize = xsize;
    p->ysize = ysize;
    p->fimage = (float *)malloc(3*xsize*ysize*sizeof(float));
  } else if (strcmp(type,"fz") == 0) {
    sprintf(p->type,"fz");
    p->itype = C_IMAGE_FZ;
    p->xsize = xsize;
    p->ysize = ysize;
    p->fimage = (float *)malloc(xsize*ysize*sizeof(float));
  } else {
    fprintf(stderr,"unknown image type %s\n",type);
  }

  return (void *)p;
}

void *image_new_rgb(int xsize, int ysize)
{
  ty_image *p;

  p = (ty_image *)malloc(sizeof(ty_image));
  p->image = NULL;
  p->fimage = NULL;
  sprintf(p->type,"rgb");
  p->itype = C_IMAGE_RGB;
  p->xsize = xsize;
  p->ysize = ysize;
  p->image = (unsigned char *)malloc(3*xsize*ysize*sizeof(unsigned char));

  return (void *)p;
}

#if 0
void *floatimage_new_rgb(int xsize, int ysize)
{
  ty_floatimage *p;

  p = (ty_floatimage *)malloc(sizeof(ty_floatimage));
  p->image = NULL;
  sprintf(p->type,"frgb");
  p->itype = C_IMAGE_FRGB;
  p->xsize = xsize;
  p->ysize = ysize;
  p->image = (float *)malloc(3*xsize*ysize*sizeof(float));

  return (void *)p;
}
#endif

void *floatimage_new_rgb(int xsize, int ysize)
{
  ty_image *p;

  p = (ty_image *)malloc(sizeof(ty_image));
  p->image = NULL;
  p->fimage = NULL;
  sprintf(p->type,"frgb");
  p->itype = C_IMAGE_FRGB;
  p->xsize = xsize;
  p->ysize = ysize;
  p->fimage = (float *)malloc(3*xsize*ysize*sizeof(float));

  return (void *)p;
}

void *image_new_frgb(int xsize, int ysize)
{
  ty_image *p;

  p = (ty_image *)malloc(sizeof(ty_image));
  p->image = NULL;
  p->fimage = NULL;
  sprintf(p->type,"frgb");
  p->itype = C_IMAGE_FRGB;
  p->xsize = xsize;
  p->ysize = ysize;
  p->fimage = (float *)malloc(3*xsize*ysize*sizeof(float));

  return (void *)p;
}

void *image_new_g(int xsize, int ysize)
{
  ty_image *p;

  p = (ty_image *)malloc(sizeof(ty_image));
  p->image = NULL;
  p->fimage = NULL;
  sprintf(p->type,"g");
  p->itype = C_IMAGE_G;
  p->xsize = xsize;
  p->ysize = ysize;
  p->image = (unsigned char *)malloc(xsize*ysize*sizeof(unsigned char));

  return (void *)p;
}

void *image_new_rgba(int xsize, int ysize)
{
  ty_image *p;

  p = (ty_image *)malloc(sizeof(ty_image));
  p->image = NULL;
  p->fimage = NULL;
  sprintf(p->type,"rgba");
  p->itype = C_IMAGE_RGBA;
  p->xsize = xsize;
  p->ysize = ysize;
  p->image = (unsigned char *)malloc(4*xsize*ysize*sizeof(unsigned char));

  return (void *)p;
}

#if 0
// XXXX We need to rewrite image_free() because we cannot delete
// XXXX the external buffer. We need "int isexternalbuffer;"
// XXXX in the structure.
void *image_new_rgba_extbuffer(int xsize, int ysize, unsigned char *buffer)
{
  ty_image *p;

  p = (ty_image *)malloc(sizeof(ty_image));
  p->image = NULL;
  p->fimage = NULL;
  p->type = C_IMAGE_RGBA;
  p->xsize = xsize;
  p->ysize = ysize;
  p->image = buffer;

  return (void *)p;
}
#endif

void image_free(void *pp)
{
  ty_image *p;

  p = pp;
  if (p->image != NULL) {
    free(p->image);
  }
  if (p->fimage != NULL) {
    free(p->fimage);
  }
  free(p);
}

char *image_get_type(void *pp)
{
  ty_image *p;

  p = pp;
  return p->type;
#if 0
  // XXXX No code used this.
  if (p->type == C_IMAGE_RGB) return global_image_type_rgb;
  if (p->type == C_IMAGE_G) return global_image_type_g;
  if (p->type == C_IMAGE_RGBA) return global_image_type_rgba;
  return NULL;
#endif
}

int image_get_xsize(void *pp)
{
  ty_image *p;

  p = pp;
  return p->xsize;
}

int image_get_ysize(void *pp)
{
  ty_image *p;

  p = pp;
  return p->ysize;
}

unsigned char *image_get_buffer(void *pp)
{
  ty_image *p;

  p = pp;
  if (p->fimage != NULL) return (unsigned char *)(p->fimage);
  return p->image;
}

int image_file_readline(FILE *in, char *s)
{
  int c,i;

  i = 0;
  for(;;) {
    c = getc(in);
    if (c == EOF) {
      return 0;
    }
    if (c == '\n') {
      s[i] = '\0';
      return 1;
    }
    s[i] = c;
    i++;
  }
}

void *image_load_file(char *path)
{
  ty_image *p;
  char *s;
  // int colors;
  int maxvalue;
  FILE *fp;
  char **sa;
  int sn;
  int i;
  int state;

  p = (ty_image *)malloc(sizeof(ty_image));
  p->image = NULL;
  p->fimage = NULL;
  if (p == NULL) {
    fprintf(stderr,"Could not malloc p\n");
    return NULL;
  }

  s = (char *)malloc(200*sizeof(char));
  if (s == NULL) {
    fprintf(stderr,"Could not malloc s\n");
    free(p);
    return NULL;
  }

  // fprintf(stderr,"image_load: path = %s\n",path);
  if ((fp = fopen(path,"r")) == NULL) {
    fprintf(stderr,"Could not open file %s for reading\n",path);
    free(s);
    free(p);
    return NULL;
  }


#if 0
  // Earlier header reading algorithm. Failed with "256\n256" size
  // definition. Kept here for a while for asking why sscanf()
  // failed badly.

  // I have used this successfully for reading the whole line.
  // It does not work here.
  // XXXX fscanf(fp,"%[^\n]",s);

  // This works but failed for "256\n256" size definition.
  do {
    image_file_readline(fp,s);
    // fscanf(fp,"%[^\n]\n",s);
    // fprintf(stderr,"%s\n",s);
  } while (s[0] == '#');
  if (strncmp(s,"P5",2) == 0) {
    p->type = C_IMAGE_G;
  } else if (strncmp(s,"P6",2) == 0) {
    p->type = C_IMAGE_RGB;
  } else if (strncmp(s,"PA6",3) == 0) {
    p->type = C_IMAGE_RGBA;
  } else {
    p->type = -1;
    free(s);
    free(p);
    fclose(fp);
    return NULL;
  }
  do {
    image_file_readline(fp,s);
    // read_line(stdin,&s);
    // fscanf(fp,"%[^\n]\n",s);
    // fprintf(stderr,"%s\n",s);
  } while (s[0] == '#');
#if 1
  // xsize and ysize may be in different lines.
  (void)sscanf(s,"%i %i",&(p->xsize),&(p->ysize));
#else
  // Does not work. We cannot read the new line if we have "128 128",
  // but we must read if we have "128\n128". sscanf fails badly.
  (void)sscanf(s,"%i",&(p->xsize));
  // Assume we have no comments between xsize and ysize.
  (void)sscanf(s,"%i",&(p->ysize));
#endif
  do {
    image_file_readline(fp,s);
    // read_line(stdin,&s);
    // fscanf(fp,"%[^\n]",s); // XXX no \n must be here but it must be in above!? 
    // fprintf(stderr,"%s\n",s);
  } while (s[0] == '#');
  (void)sscanf(s,"%i",&colors);
  free(s);
#endif


  // New ppm header reading algorithm:
  // 1. Read a line.
  // 2. If it starts with "#", skip the line and go to 1.
  // 3. Split the line to tokens separated by space. Put them to fifo.
  // 4. Read fifo until empty.
  // 5. If state == 0 then expect P5, P6 etc.
  // 6. If state == 1 then expect xsize
  // 7. If state == 2 then expect ysize
  // 8. If state == 3 then expect colors
  // 9. If token was read correctly, increase the state.
  // 10. If state == 4 then read the image data.
  state = 0;
  do {
    image_file_readline(fp,s);
    if (s[0] != '#') {
      image_split_string(s,&sa,&sn);
      for (i = 0; i < sn; i++) {
	if (state == 0) {
	  if (strncmp(sa[i],"P5",2) == 0) {
	    sprintf(p->type,"g");
	    p->itype = C_IMAGE_G;
	  } else if (strncmp(sa[i],"P6",2) == 0) {
	    sprintf(p->type,"rgb");
	    p->itype = C_IMAGE_RGB;
	  } else if (strncmp(sa[i],"PA6",3) == 0) {
	    sprintf(p->type,"rgba");
	    p->itype = C_IMAGE_RGBA;
	  } else {
	    sprintf(p->type,"");
	    p->itype = -1;
	    free(s);
	    free(p);
	    fclose(fp);
	    return NULL;
	  }
	  state = 1;
	} else if (state == 1) {
	  sscanf(sa[i],"%i",&(p->xsize));
	  state = 2;
	} else if (state == 2) {
	  sscanf(sa[i],"%i",&(p->ysize));
	  state = 3;
	} else if (state == 3) {
	  sscanf(sa[i],"%i",&maxvalue);
	  if (maxvalue != 255) {
	    fprintf(stderr,"PNM maxvalue %i != 255\n",maxvalue);
	  }
	  state = 4;
	}
      }
      for (i = 0; i < sn; i++) {
	free(sa[i]);
      }
      free(sa);
    }
  } while (state < 4);

#if 0
  switch (p->itype) {
  case C_IMAGE_G:
    p->image = (unsigned char *)malloc(p->xsize*p->ysize*sizeof(unsigned char));
    if (fread(p->image,sizeof(unsigned char),p->xsize*p->ysize,fp)
	!= p->xsize*p->ysize) {
      fprintf(stderr,"image_load: unable to read the actual image data\n");
      free(s);
      free(p);
      fclose(fp);
      return NULL;
    }
    break;
  case C_IMAGE_RGB:
    p->image = (unsigned char *)malloc(3*p->xsize*p->ysize*sizeof(unsigned char));
    if (fread(p->image,sizeof(unsigned char),3*p->xsize*p->ysize,fp)
	!= 3*p->xsize*p->ysize) {
      fprintf(stderr,"image_load: unable to read the actual image data\n");
      free(s);
      free(p);
      fclose(fp);
      return NULL;
    }
    break;
  case C_IMAGE_RGBA:
    p->image = (unsigned char *)malloc(4*p->xsize*p->ysize*sizeof(unsigned char));
    if (fread(p->image,sizeof(unsigned char),4*p->xsize*p->ysize,fp)
	!= 4*p->xsize*p->ysize) {
      fprintf(stderr,"image_load: unable to read the actual image data\n");
      free(s);
      free(p);
      fclose(fp);
      return NULL;
    }
    break;
  }
#endif

  if (strcmp(p->type,"g") == 0) {
    p->image = (unsigned char *)malloc(p->xsize*p->ysize*sizeof(unsigned char));
    if (fread(p->image,sizeof(unsigned char),p->xsize*p->ysize,fp)
	!= p->xsize*p->ysize) {
      fprintf(stderr,"image_load: unable to read the actual image data\n");
      free(s);
      free(p);
      fclose(fp);
      return NULL;
    }
  } else if (strcmp(p->type,"rgb") == 0) {
    p->image = (unsigned char *)malloc(3*p->xsize*p->ysize*sizeof(unsigned char));
    if (fread(p->image,sizeof(unsigned char),3*p->xsize*p->ysize,fp)
	!= 3*p->xsize*p->ysize) {
      fprintf(stderr,"image_load: unable to read the actual image data\n");
      free(s);
      free(p);
      fclose(fp);
      return NULL;
    }
  } else if (strcmp(p->type,"rgba") == 0) {
    p->image = (unsigned char *)malloc(4*p->xsize*p->ysize*sizeof(unsigned char));
    if (fread(p->image,sizeof(unsigned char),4*p->xsize*p->ysize,fp)
	!= 4*p->xsize*p->ysize) {
      fprintf(stderr,"image_load: unable to read the actual image data\n");
      free(s);
      free(p);
      fclose(fp);
      return NULL;
    }
  }

  fclose(fp);

  return (void *)p;
}

#if 0
// Delete this after testing more that the new header parser works.

// Failed to load image with "<xsize>\n<ysize>".
// ppm.gz pgm.gz pnm.gz
void *image_load_file_comp(char *path)
{
  ty_image *p;
  char *s;
  int colors;
  FILE *fp;
  char *cmd;

  p = (ty_image *)malloc(sizeof(ty_image));
  if (p == NULL) {
    fprintf(stderr,"Could not malloc p\n");
    return NULL;
  }

  s = (char *)malloc(200*sizeof(char));
  if (s == NULL) {
    fprintf(stderr,"Could not malloc s\n");
    free(p);
    return NULL;
  }

  cmd = (char *)malloc((strlen(path)+10)*sizeof(char));
  sprintf(cmd,"zcat %s",path);

  // fprintf(stderr,"image_load: path = %s\n",path);
  // if ((fp = fopen(path,"r")) == NULL) {
  if ((fp = popen(cmd,"r")) == NULL) {
    fprintf(stderr,"Could not open file %s for reading\n",path);
    free(s);
    free(p);
    return NULL;
  }
  // XXXX fscanf(fp,"%[^\n]",s);
  do {
    image_file_readline(fp,s);
    // fscanf(fp,"%[^\n]\n",s);
    // fprintf(stderr,"%s\n",s);
  } while (s[0] == '#');
  if (strncmp(s,"P5",2) == 0) {
    p->type = C_IMAGE_G;
  } else if (strncmp(s,"P6",2) == 0) {
    p->type = C_IMAGE_RGB;
  } else if (strncmp(s,"PA6",3) == 0) {
    p->type = C_IMAGE_RGBA;
  } else {
    p->type = -1;
    free(s);
    free(p);
    fclose(fp);
    return NULL;
  }
  do {
    image_file_readline(fp,s);
    // read_line(stdin,&s);
    // fscanf(fp,"%[^\n]\n",s);
    // fprintf(stderr,"%s\n",s);
  } while (s[0] == '#');
#if 1
  // xsize and ysize may be in different lines.
  (void)sscanf(s,"%i %i",&(p->xsize),&(p->ysize));
#else
  // Does not work. We cannot read the new line if we have "128 128",
  // but we must read if we have "128\n128". sscanf fails badly.
  (void)sscanf(s,"%i",&(p->xsize));
  // Assume we have no comments between xsize and ysize.
  (void)sscanf(s,"%i",&(p->ysize));
#endif
  do {
    image_file_readline(fp,s);
    // read_line(stdin,&s);
    // fscanf(fp,"%[^\n]",s); // XXX no \n must be here but it must be in above!? 
    // fprintf(stderr,"%s\n",s);
  } while (s[0] == '#');
  (void)sscanf(s,"%i",&colors);

  switch (p->type) {
  case C_IMAGE_G:
    p->image = (unsigned char *)malloc(p->xsize*p->ysize*sizeof(unsigned char));
    if (fread(p->image,sizeof(unsigned char),p->xsize*p->ysize,fp)
	!= p->xsize*p->ysize) {
      fprintf(stderr,"image_load: unable to read the actual image data\n");
      free(s);
      free(p);
      fclose(fp);
      return NULL;
    }
    break;
  case C_IMAGE_RGB:
    p->image = (unsigned char *)malloc(3*p->xsize*p->ysize*sizeof(unsigned char));
    if (fread(p->image,sizeof(unsigned char),3*p->xsize*p->ysize,fp)
	!= 3*p->xsize*p->ysize) {
      fprintf(stderr,"image_load: unable to read the actual image data\n");
      free(s);
      free(p);
      fclose(fp);
      return NULL;
    }
    break;
  case C_IMAGE_RGBA:
    p->image = (unsigned char *)malloc(4*p->xsize*p->ysize*sizeof(unsigned char));
    if (fread(p->image,sizeof(unsigned char),4*p->xsize*p->ysize,fp)
	!= 4*p->xsize*p->ysize) {
      fprintf(stderr,"image_load: unable to read the actual image data\n");
      free(s);
      free(p);
      fclose(fp);
      return NULL;
    }
    break;
  }

  // fclose(fp);
  pclose(fp);
  free(cmd);
  free(s);

  return (void *)p;
}
#endif


// New header parser.
// ppm.gz pgm.gz pnm.gz
void *image_load_file_comp(char *path)
{
  ty_image *p;
  char *s;
  int maxvalue;
  FILE *fp;
  char *cmd;
  char **sa;
  int sn;
  int i;
  int state;

  p = (ty_image *)malloc(sizeof(ty_image));
  if (p == NULL) {
    fprintf(stderr,"Could not malloc p\n");
    return NULL;
  }
  p->image = NULL;
  p->fimage = NULL;

  s = (char *)malloc(2000*sizeof(char));
  if (s == NULL) {
    fprintf(stderr,"Could not malloc s\n");
    free(p);
    return NULL;
  }

  cmd = (char *)malloc((strlen(path)+10)*sizeof(char));
  sprintf(cmd,"zcat %s",path);

  // fprintf(stderr,"image_load: path = %s\n",path);
  // if ((fp = fopen(path,"r")) == NULL) {
  if ((fp = popen(cmd,"r")) == NULL) {
    fprintf(stderr,"Could not open file %s for reading\n",path);
    free(s);
    free(p);
    return NULL;
  }

  // New ppm header reading algorithm:
  // 1. Read a line.
  // 2. If it starts with "#", skip the line and go to 1.
  // 3. Split the line to tokens separated by space. Put them to fifo.
  // 4. Read fifo until empty.
  // 5. If state == 0 then expect P5, P6 etc.
  // 6. If state == 1 then expect xsize
  // 7. If state == 2 then expect ysize
  // 8. If state == 3 then expect colors
  // 9. If token was read correctly, increase the state.
  // 10. If state == 4 then read the image data.
  state = 0;
  do {
    image_file_readline(fp,s);
    if (s[0] != '#') {
      image_split_string(s,&sa,&sn);
      for (i = 0; i < sn; i++) {
	if (state == 0) {
	  if (strncmp(sa[i],"P5",2) == 0) {
	    sprintf(p->type,"g");
	    p->itype = C_IMAGE_G;
	  } else if (strncmp(sa[i],"P6",2) == 0) {
	    sprintf(p->type,"rgb");
	    p->itype = C_IMAGE_RGB;
	  } else if (strncmp(sa[i],"PA6",3) == 0) {
	    sprintf(p->type,"rgba");
	    p->itype = C_IMAGE_RGBA;
	  } else {
	    sprintf(p->type,"");
	    p->itype = -1;
	    free(s);
	    free(p);
	    fclose(fp);
	    return NULL;
	  }
	  state = 1;
	} else if (state == 1) {
	  sscanf(sa[i],"%i",&(p->xsize));
	  state = 2;
	} else if (state == 2) {
	  sscanf(sa[i],"%i",&(p->ysize));
	  state = 3;
	} else if (state == 3) {
	  sscanf(sa[i],"%i",&maxvalue);
	  if (maxvalue != 255) {
	    fprintf(stderr,"PNM maxvalue %i != 255\n",maxvalue);
	  }
	  state = 4;
	}
      }
      for (i = 0; i < sn; i++) {
	free(sa[i]);
      }
      free(sa);
    }
  } while (state < 4);


  if (strcmp(p->type,"g") == 0) {
    p->image = (unsigned char *)malloc(p->xsize*p->ysize*sizeof(unsigned char));
    if (fread(p->image,sizeof(unsigned char),p->xsize*p->ysize,fp)
	!= p->xsize*p->ysize) {
      fprintf(stderr,"image_load: unable to read the actual image data\n");
      free(s);
      free(p);
      fclose(fp);
      return NULL;
    }
  } else if (strcmp(p->type,"rgb") == 0) {
    p->image = (unsigned char *)malloc(3*p->xsize*p->ysize*sizeof(unsigned char));
    if (fread(p->image,sizeof(unsigned char),3*p->xsize*p->ysize,fp)
	!= 3*p->xsize*p->ysize) {
      fprintf(stderr,"image_load: unable to read the actual image data\n");
      free(s);
      free(p);
      fclose(fp);
      return NULL;
    }
  } else if (strcmp(p->type,"rgba") == 0) {
    p->image = (unsigned char *)malloc(4*p->xsize*p->ysize*sizeof(unsigned char));
    if (fread(p->image,sizeof(unsigned char),4*p->xsize*p->ysize,fp)
	!= 4*p->xsize*p->ysize) {
      fprintf(stderr,"image_load: unable to read the actual image data\n");
      free(s);
      free(p);
      fclose(fp);
      return NULL;
    }
  }

  // fclose(fp);
  pclose(fp);
  free(cmd);
  free(s);

  return (void *)p;
}

// CONTINUE  type vs itype
#if 1
// Reads one scanline at a time.
void *image_load_file_jpeg(char *path)
{
  ty_image *p;
  char *s;
  FILE *fp;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  int y;
  unsigned char *imagerowptr[1];

  p = (ty_image *)malloc(sizeof(ty_image));
  if (p == NULL) {
    fprintf(stderr,"Could not malloc p\n");
    return NULL;
  }
  p->image = NULL;
  p->fimage = NULL;

  s = (char *)malloc(200*sizeof(char));
  if (s == NULL) {
    fprintf(stderr,"Could not malloc s\n");
    free(p);
    return NULL;
  }

  // fprintf(stderr,"image_load: path = %s\n",path);
  if ((fp = fopen(path,"r")) == NULL) {
    fprintf(stderr,"Could not open file %s for reading\n",path);
    free(s);
    free(p);
    return NULL;
  }

  cinfo.err = jpeg_std_error (&jerr);
  jpeg_create_decompress (&cinfo);
  jpeg_stdio_src (&cinfo, fp);
  (void) jpeg_read_header (&cinfo, TRUE);
  jpeg_start_decompress (&cinfo);

  if (cinfo.output_components == 1) {
    sprintf(p->type,"g");
    p->itype = C_IMAGE_G;
  } else if (cinfo.output_components == 3) {
    sprintf(p->type,"rgb");
    p->itype = C_IMAGE_RGB;
  } else {
    fprintf(stderr,"jpeg output components = %i\n",cinfo.output_components);
    exit(-1);
  }

  p->xsize = cinfo.output_width;
  p->ysize = cinfo.output_height;

  switch (p->itype) {
  case C_IMAGE_G:
    p->image = (unsigned char *)malloc(p->xsize*p->ysize*sizeof(unsigned char));
    break;
  case C_IMAGE_RGB:
    p->image = (unsigned char *)malloc(3*p->xsize*p->ysize*sizeof(unsigned char));
    break;
  }

  for (y = 0; y < p->ysize; y++) {
    switch (p->itype) {
    case C_IMAGE_G:
      imagerowptr[0] = &(p->image[y*p->xsize]);
      jpeg_read_scanlines (&cinfo, (JSAMPARRAY) imagerowptr, 1);
      break;
    case C_IMAGE_RGB:
      imagerowptr[0] = &(p->image[y*3*p->xsize]);
      jpeg_read_scanlines (&cinfo, (JSAMPARRAY) imagerowptr, 1);
      break;
    }
  }

  jpeg_finish_decompress (&cinfo);
  jpeg_destroy_decompress (&cinfo);

  fclose(fp);

  return (void *)p;
}
#endif

#if 0
// XXXX Does not work. Why? Error: "Application transferred too few scanlines".
// Reads multiple scanlines at a time.
void *image_load_file_jpeg(char *path)
{
  ty_image *p;
  char *s;
  int colors;
  FILE *fp;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  int y,ny;
  unsigned char *imagerowptr[4];
  int i;
  int total;

  p = (ty_image *)malloc(sizeof(ty_image));
  if (p == NULL) {
    fprintf(stderr,"Could not malloc p\n");
    return NULL;
  }
  p->image = NULL;
  p->fimage = NULL;

  s = (char *)malloc(200*sizeof(char));
  if (s == NULL) {
    fprintf(stderr,"Could not malloc s\n");
    free(p);
    return NULL;
  }

  // fprintf(stderr,"image_load: path = %s\n",path);
  if ((fp = fopen(path,"r")) == NULL) {
    fprintf(stderr,"Could not open file %s for reading\n",path);
    free(s);
    free(p);
    return NULL;
  }

  cinfo.err = jpeg_std_error (&jerr);
  jpeg_create_decompress (&cinfo);
  jpeg_stdio_src (&cinfo, fp);
  (void) jpeg_read_header (&cinfo, TRUE);
  jpeg_start_decompress (&cinfo);

  if (cinfo.output_components == 1) {
    p->type = C_IMAGE_G;
  } else if (cinfo.output_components == 3) {
    p->type = C_IMAGE_RGB;
  } else {
    fprintf(stderr,"jpeg output components = %i\n",cinfo.output_components);
    exit(-1);
  }

  p->xsize = cinfo.output_width;
  p->ysize = cinfo.output_height;

  switch (p->type) {
  case C_IMAGE_G:
    p->image = (unsigned char *)malloc(p->xsize*p->ysize*sizeof(unsigned char));
    break;
  case C_IMAGE_RGB:
    p->image = (unsigned char *)malloc(3*p->xsize*p->ysize*sizeof(unsigned char));
    break;
  }

  total = 0;
  for (y = 0; y < p->ysize; y += 4) {
    ny = 4;
    if (ny > p->ysize - y) ny = p->ysize - y;
    switch (p->type) {
    case C_IMAGE_G:
      for (i = 0; i < ny; i++) {
	imagerowptr[i] = &(p->image[(y+i)*p->xsize]);
      }
      jpeg_read_scanlines (&cinfo, (JSAMPARRAY) imagerowptr, ny);
      break;
    case C_IMAGE_RGB:
      for (i = 0; i < ny; i++) {
	imagerowptr[i] = &(p->image[(y+i)*3*p->xsize]);
      }
      jpeg_read_scanlines (&cinfo, (JSAMPARRAY) imagerowptr, ny);
      break;
    }
    total += ny;
  }
  fprintf(stderr,"total scanlines = %i\n",total);

  jpeg_finish_decompress (&cinfo);
  jpeg_destroy_decompress (&cinfo);

  fclose(fp);

  return (void *)p;
}
#endif


void *image_load_file_png(char *path)
{
  ty_image *p;
  ty_image *q;
  char *s;
  FILE *fp;
  png_structp pngp;
  png_infop   pngi;
  int y;
  unsigned char *imagerowptr[1];

  p = (ty_image *)malloc(sizeof(ty_image));
  if (p == NULL) {
    fprintf(stderr,"Could not malloc p\n");
    return NULL;
  }
  p->image = NULL;
  p->fimage = NULL;

  s = (char *)malloc(200*sizeof(char));
  if (s == NULL) {
    fprintf(stderr,"Could not malloc s\n");
    free(p);
    return NULL;
  }

  // fprintf(stderr,"image_load: path = %s\n",path);
  if ((fp = fopen(path,"r")) == NULL) {
    fprintf(stderr,"Could not open file %s for reading\n",path);
    free(s);
    free(p);
    return NULL;
  }

  pngp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  pngi = png_create_info_struct(pngp);
  png_init_io(pngp, fp);
  png_read_info(pngp, pngi);





  if (pngi->color_type == PNG_COLOR_TYPE_GRAY) {
    sprintf(p->type,"g");
    p->itype = C_IMAGE_G;
  } else if (pngi->color_type == PNG_COLOR_TYPE_RGB) {
    sprintf(p->type,"rgb");
    p->itype = C_IMAGE_RGB;
  } else if (pngi->color_type == PNG_COLOR_TYPE_RGBA) {
    sprintf(p->type,"rgba");
    p->itype = C_IMAGE_RGBA;
  } else {
    fprintf(stderr,"unimplemented png type %i\n",pngi->color_type);
    // exit(-1);
    png_destroy_read_struct(&pngp,&pngi,NULL);
    free(s);
    free(p);
    fclose(fp);
    return NULL;
  }

  if (pngi->bit_depth != 8) {
    fprintf(stderr,"unimplemented png bit depth %i\n",pngi->bit_depth);
    png_destroy_read_struct(&pngp,&pngi,NULL);
    free(s);
    free(p);
    fclose(fp);
    return NULL;
  }

  p->xsize = pngi->width;
  p->ysize = pngi->height;
  switch (p->itype) {
  case C_IMAGE_G:
    p->image = (unsigned char *)malloc(p->xsize*p->ysize*sizeof(unsigned char));
    break;
  case C_IMAGE_RGB:
    p->image = (unsigned char *)malloc(3*p->xsize*p->ysize*sizeof(unsigned char));
    break;
  case C_IMAGE_RGBA:
    p->image = (unsigned char *)malloc(4*p->xsize*p->ysize*sizeof(unsigned char));
    break;
  }

  for (y = 0; y < p->ysize; y++) {
    switch (p->itype) {
    case C_IMAGE_G:
      imagerowptr[0] = &(p->image[y*p->xsize]);
      png_read_rows(pngp, imagerowptr, NULL, 1);
      break;
    case C_IMAGE_RGB:
      imagerowptr[0] = &(p->image[y*3*p->xsize]);
      png_read_rows(pngp, imagerowptr, NULL, 1);
      break;
    case C_IMAGE_RGBA:
      imagerowptr[0] = &(p->image[y*4*p->xsize]);
      png_read_rows(pngp, imagerowptr, NULL, 1);
      break;
    }
  }

  // png_read_row(png_structp png_ptr,png_bytep row,png_bytep display_row);
  // png_read_image(png_structp png_ptr,png_bytepp image); // whole image

  png_read_end(pngp, pngi);

  png_destroy_read_struct(&pngp,&pngi,NULL);

  fclose(fp);

  if (p->itype == C_IMAGE_RGBA) {
    q = image_convert_to_rgb(p);
    image_free(p);
    p = q;
  }

  return (void *)p;
}


// XXXX float type
void image_save_file(void *pp, char *s)
{
  ty_image *p;
  FILE *fp;

  p = pp;
  fp = fopen(s,"w");
  if (fp == NULL) {
    fprintf(stderr,"Could not open file %s for writing\n",s);
    exit(-1);
  }
  switch (p->itype) {
  case C_IMAGE_RGB:
    (void)fprintf(fp,"P6\n");
    (void)fprintf(fp,"%i %i\n",p->xsize,p->ysize);
    (void)fprintf(fp,"255\n");
    if (fwrite(p->image,sizeof(unsigned char),3*p->xsize*p->ysize,fp)
        != 3*p->xsize*p->ysize) {
      fprintf(stderr,"image_save: unable to write the actual image data\n");
    }
    break;
  case C_IMAGE_G:
    (void)fprintf(fp,"P5\n");
    (void)fprintf(fp,"%i %i\n",p->xsize,p->ysize);
    (void)fprintf(fp,"255\n");
    if (fwrite(p->image,sizeof(unsigned char),p->xsize*p->ysize,fp)
        != p->xsize*p->ysize) {
      fprintf(stderr,"image_save: unable to write the actual image data\n");
    }
    break;
  case C_IMAGE_RGBA:
    (void)fprintf(fp,"PA6\n");
    (void)fprintf(fp,"%i %i\n",p->xsize,p->ysize);
    (void)fprintf(fp,"255\n");
    if (fwrite(p->image,sizeof(unsigned char),4*p->xsize*p->ysize,fp)
        != 4*p->xsize*p->ysize) {
      fprintf(stderr,"image_save: unable to write the actual image data\n");
    }
    break;
  }
  fclose(fp);
}

// XXXX float type
void image_save_stdout(void *pp)
{
  ty_image *p;

  p = pp;
  switch (p->itype) {
  case C_IMAGE_RGB:
    (void)fprintf(stdout,"P6\n");
    (void)fprintf(stdout,"%i %i\n",p->xsize,p->ysize);
    (void)fprintf(stdout,"255\n");
    if (fwrite(p->image,sizeof(unsigned char),3*p->xsize*p->ysize,stdout)
        != 3*p->xsize*p->ysize) {
      fprintf(stderr,"image_save: unable to write the actual image data\n");
    }
    break;
  case C_IMAGE_G:
    (void)fprintf(stdout,"P5\n");
    (void)fprintf(stdout,"%i %i\n",p->xsize,p->ysize);
    (void)fprintf(stdout,"255\n");
    if (fwrite(p->image,sizeof(unsigned char),p->xsize*p->ysize,stdout)
        != p->xsize*p->ysize) {
      fprintf(stderr,"image_save: unable to write the actual image data\n");
    }
    break;
  case C_IMAGE_RGBA:
    (void)fprintf(stdout,"PA6\n");
    (void)fprintf(stdout,"%i %i\n",p->xsize,p->ysize);
    (void)fprintf(stdout,"255\n");
    if (fwrite(p->image,sizeof(unsigned char),4*p->xsize*p->ysize,stdout)
        != 4*p->xsize*p->ysize) {
      fprintf(stderr,"image_save: unable to write the actual image data\n");
    }
    break;
  }
  fflush(stdout);
}

void image_write2file_rgb_from_rgba(void *pp, char *s)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  float a;

  p = pp;
  if (p->itype != C_IMAGE_RGBA) return;

  q = image_new_rgb(p->xsize,p->ysize);
  for (y = 0; y < p->ysize; y++) {
    for (x = 0; x < p->xsize; x++) {
      q->image[3*(y*q->xsize + x) + 0] = p->image[4*(y*p->xsize + x) + 0];
      q->image[3*(y*q->xsize + x) + 1] = p->image[4*(y*p->xsize + x) + 1];
      q->image[3*(y*q->xsize + x) + 2] = p->image[4*(y*p->xsize + x) + 2];
    }
  }
  image_save_file(q,s);
  image_free(q);

}

// void ddsimage_write2file_rgb_premultipliedalpha(void *pp, char *s)
void image_write2file_rgb_premultipliedalpha(void *pp, char *s)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  float a;

  p = pp;
  if (p->itype != C_IMAGE_RGBA) return;

  q = image_new_rgb(p->xsize,p->ysize);
  for (y = 0; y < p->ysize; y++) {
    for (x = 0; x < p->xsize; x++) {
      a = p->image[4*(y*p->xsize + x) + 3]/255.0;
      q->image[3*(y*q->xsize + x) + 0] = a*p->image[4*(y*p->xsize + x) + 0];
      q->image[3*(y*q->xsize + x) + 1] = a*p->image[4*(y*p->xsize + x) + 1];
      q->image[3*(y*q->xsize + x) + 2] = a*p->image[4*(y*p->xsize + x) + 2];
    }
  }
  image_save_file(q,s);
  image_free(q);

}

// void ddsimage_write2file_rgb_premultipliedalpha_speedtree(ty_ddsimage *p, char *s)
void image_write2file_rgb_premultipliedalpha_speedtree(void *pp, char *s)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  float a;

  p = pp;
  if (p->itype != C_IMAGE_RGBA) return;

  q = image_new_rgb(p->xsize,p->ysize);
  for (y = 0; y < p->ysize; y++) {
    for (x = 0; x < p->xsize; x++) {
      a = p->image[4*(y*p->xsize + x) + 3]/255.0;
      if (a > 0.3333) a = 1.0;
      q->image[3*(y*q->xsize + x) + 0] = a*p->image[4*(y*p->xsize + x) + 0];
      q->image[3*(y*q->xsize + x) + 1] = a*p->image[4*(y*p->xsize + x) + 1];
      q->image[3*(y*q->xsize + x) + 2] = a*p->image[4*(y*p->xsize + x) + 2];
    }
  }
  image_save_file(q,s);
  image_free(q);

}

// void ddsimage_write2file_alpha(ty_ddsimage *p, char *s)
void image_write2file_alpha(void *pp, char *s)
{
  ty_image *p;
  ty_image *q;
  int x,y;

  p = pp;
  if (p->itype != C_IMAGE_RGBA) return;

  q = image_new_g(p->xsize,p->ysize);
  for (y = 0; y < p->ysize; y++) {
    for (x = 0; x < p->xsize; x++) {
      q->image[y*p->xsize + x] = p->image[4*(y*p->xsize + x) + 3];
    }
  }
  image_save_file(q,s);
  image_free(q);

}

// image_put()
void image_put_rgba_float(void *pp, int x, int y, float *rgba)
{
  ty_image *p;
  int r,g,b,a;

  p = pp;
  switch (p->itype) {
  case C_IMAGE_RGB:
    if (rgba[0] < 0.0) rgba[0] = 0.0;
    if (rgba[1] < 0.0) rgba[1] = 0.0;
    if (rgba[2] < 0.0) rgba[2] = 0.0;
    if (rgba[0] > 1.0) rgba[0] = 1.0;
    if (rgba[1] > 1.0) rgba[1] = 1.0;
    if (rgba[2] > 1.0) rgba[2] = 1.0;
    r = floor(rgba[0]*255.99);
    g = floor(rgba[1]*255.99);
    b = floor(rgba[2]*255.99);
    p->image[3*(y*p->xsize + x) + 0] = (unsigned char)r;
    p->image[3*(y*p->xsize + x) + 1] = (unsigned char)g;
    p->image[3*(y*p->xsize + x) + 2] = (unsigned char)b;
    break;
  case C_IMAGE_G:
    if (rgba[1] < 0.0) rgba[1] = 0.0;
    if (rgba[1] > 1.0) rgba[1] = 1.0;
    g = floor(rgba[1]*255.99);
    p->image[y*p->xsize + x] = (unsigned char)g;
    break;
  case C_IMAGE_RGBA:
    if (rgba[0] < 0.0) rgba[0] = 0.0;
    if (rgba[1] < 0.0) rgba[1] = 0.0;
    if (rgba[2] < 0.0) rgba[2] = 0.0;
    if (rgba[3] < 0.0) rgba[3] = 0.0;
    if (rgba[0] > 1.0) rgba[0] = 1.0;
    if (rgba[1] > 1.0) rgba[1] = 1.0;
    if (rgba[2] > 1.0) rgba[2] = 1.0;
    if (rgba[3] > 1.0) rgba[3] = 1.0;
    r = floor(rgba[0]*255.99);
    g = floor(rgba[1]*255.99);
    b = floor(rgba[2]*255.99);
    a = floor(rgba[3]*255.99);
    p->image[4*(y*p->xsize + x) + 0] = (unsigned char)r;
    p->image[4*(y*p->xsize + x) + 1] = (unsigned char)g;
    p->image[4*(y*p->xsize + x) + 2] = (unsigned char)b;
    p->image[4*(y*p->xsize + x) + 3] = (unsigned char)a;
    break;
  }
}

void image_get_rgb(void *pp, int x, int y, int *r, int *g, int *b)
{
  ty_image *p;

  p = pp;
  switch (p->itype) {
  case C_IMAGE_RGB:
    *r = p->image[3*(y*p->xsize + x) + 0];
    *g = p->image[3*(y*p->xsize + x) + 1];
    *b = p->image[3*(y*p->xsize + x) + 2];
    break;
  case C_IMAGE_G:
    *r = p->image[y*p->xsize + x];
    *g = p->image[y*p->xsize + x];
    *b = p->image[y*p->xsize + x];
    break;
  case C_IMAGE_RGBA:
    *r = p->image[4*(y*p->xsize + x) + 0];
    *g = p->image[4*(y*p->xsize + x) + 1];
    *b = p->image[4*(y*p->xsize + x) + 2];
    break;
  }
}

void image_put_rgb(void *pp, int x, int y, int r, int g, int b)
{
  ty_image *p;

  p = pp;
  switch (p->itype) {
  case C_IMAGE_RGB:
    p->image[3*(y*p->xsize + x) + 0] = (unsigned char)r;
    p->image[3*(y*p->xsize + x) + 1] = (unsigned char)g;
    p->image[3*(y*p->xsize + x) + 2] = (unsigned char)b;
    break;
  case C_IMAGE_G:
    p->image[y*p->xsize + x] = (unsigned char)g;
    break;
  case C_IMAGE_RGBA:
    p->image[4*(y*p->xsize + x) + 0] = (unsigned char)r;
    p->image[4*(y*p->xsize + x) + 1] = (unsigned char)g;
    p->image[4*(y*p->xsize + x) + 2] = (unsigned char)b;
    p->image[4*(y*p->xsize + x) + 3] = (unsigned char)255;
    break;
  }
}

void image_get_rgba(void *pp, int x, int y, int *r, int *g, int *b, int *a)
{
  ty_image *p;

  p = pp;
  switch (p->itype) {
  case C_IMAGE_RGB:
    *r = p->image[3*(y*p->xsize + x) + 0];
    *g = p->image[3*(y*p->xsize + x) + 1];
    *b = p->image[3*(y*p->xsize + x) + 2];
    *a = 255;
    break;
  case C_IMAGE_G:
    *r = p->image[y*p->xsize + x];
    *g = *r;
    *b = *r;
    *a = 255;
    break;
  case C_IMAGE_RGBA:
    *r = p->image[4*(y*p->xsize + x) + 0];
    *g = p->image[4*(y*p->xsize + x) + 1];
    *b = p->image[4*(y*p->xsize + x) + 2];
    *a = p->image[4*(y*p->xsize + x) + 3];
    break;
  }
}

void image_put_rgba(void *pp, int x, int y, int r, int g, int b, int a)
{
  ty_image *p;

  p = pp;
  switch (p->itype) {
  case C_IMAGE_RGB:
    p->image[3*(y*p->xsize + x) + 0] = r;
    p->image[3*(y*p->xsize + x) + 1] = g;
    p->image[3*(y*p->xsize + x) + 2] = b;
    break;
  case C_IMAGE_G:
    p->image[y*p->xsize + x] = g;
    break;
  case C_IMAGE_RGBA:
    p->image[4*(y*p->xsize + x) + 0] = r;
    p->image[4*(y*p->xsize + x) + 1] = g;
    p->image[4*(y*p->xsize + x) + 2] = b;
    p->image[4*(y*p->xsize + x) + 3] = a;
    break;
  }
}

void image_put_fz(void *pp, int x, int y, float z)
{
  ty_image *p;

  p = pp;
  p->fimage[y*p->xsize + x] = z;
}

float image_get_fz(void *pp, int x, int y)
{
  ty_image *p;

  p = pp;
  return p->fimage[y*p->xsize + x];
}

void *image_convert_to_rgb(void *pp)
{
  ty_image *p;
  ty_image *q;
  int x,y;

  p = pp;
  q = image_new_rgb(p->xsize,p->ysize);
  for (y = 0; y < p->ysize; y++) {
    for (x = 0; x < p->xsize; x++) {
      switch (p->itype) {
      case C_IMAGE_RGB:
	// memcpy?
	q->image[3*(y*q->xsize + x) + 0] = p->image[3*(y*p->xsize + x) + 0];
	q->image[3*(y*q->xsize + x) + 1] = p->image[3*(y*p->xsize + x) + 1];
	q->image[3*(y*q->xsize + x) + 2] = p->image[3*(y*p->xsize + x) + 2];
	break;
      case C_IMAGE_G:
	// read g; put g; q++; put g; q++; put g; q++; p++;
	q->image[3*(y*q->xsize + x) + 0] = p->image[1*(y*p->xsize + x) + 0];
	q->image[3*(y*q->xsize + x) + 1] = p->image[1*(y*p->xsize + x) + 0];
	q->image[3*(y*q->xsize + x) + 2] = p->image[1*(y*p->xsize + x) + 0];
	break;
      case C_IMAGE_RGBA:
	// read r; p++; read g; p++; read b; pp++; put r; q++; put g; q++; put b; q++;
	q->image[3*(y*q->xsize + x) + 0] = p->image[4*(y*p->xsize + x) + 0];
	q->image[3*(y*q->xsize + x) + 1] = p->image[4*(y*p->xsize + x) + 1];
	q->image[3*(y*q->xsize + x) + 2] = p->image[4*(y*p->xsize + x) + 2];
	break;
      }
    }
  }
  return q;
}

// XXXX Use memcpy to copy the buffers.
void image_copy_buffer(void *pp, unsigned char *buffer)
{
  ty_image *p;
  int x,y;

  p = pp;
  for (y = 0; y < p->ysize; y++) {
    for (x = 0; x < p->xsize; x++) {
      switch (p->itype) {
      case C_IMAGE_RGB:
	p->image[3*(y*p->xsize + x) + 0] = buffer[3*(y*p->xsize + x) + 0];
	p->image[3*(y*p->xsize + x) + 1] = buffer[3*(y*p->xsize + x) + 1];
	p->image[3*(y*p->xsize + x) + 2] = buffer[3*(y*p->xsize + x) + 2];
	break;
      case C_IMAGE_G:
	p->image[1*(y*p->xsize + x) + 0] = buffer[1*(y*p->xsize + x) + 0];
	break;
      case C_IMAGE_RGBA:
	p->image[4*(y*p->xsize + x) + 0] = buffer[4*(y*p->xsize + x) + 0];
	p->image[4*(y*p->xsize + x) + 1] = buffer[4*(y*p->xsize + x) + 1];
	p->image[4*(y*p->xsize + x) + 2] = buffer[4*(y*p->xsize + x) + 2];
	p->image[4*(y*p->xsize + x) + 3] = buffer[4*(y*p->xsize + x) + 3];
	break;
      }
    }
  }
}

#if 0
// XXXX Does not work for all image type combinations. Fix it.
void image_scale(void *pp, void *qq)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;

  p = pp;
  q = qq;
  for (y = 0; y < q->ysize; y++) {
    for (x = 0; x < q->xsize; x++) {
      xx = (int)((float)x*(float)(p->xsize)/(float)(q->xsize)); 
      yy = (int)((float)y*(float)(p->ysize)/(float)(q->ysize)); 
      switch (p->type) {
      case C_IMAGE_RGB:
	q->image[3*(y*q->xsize + x) + 0] = p->image[3*(yy*p->xsize + xx) + 0];
	q->image[3*(y*q->xsize + x) + 1] = p->image[3*(yy*p->xsize + xx) + 1];
	q->image[3*(y*q->xsize + x) + 2] = p->image[3*(yy*p->xsize + xx) + 2];
	break;
      case C_IMAGE_G:
	q->image[3*(y*q->xsize + x) + 0] = p->image[1*(yy*p->xsize + xx) + 0];
	q->image[3*(y*q->xsize + x) + 1] = p->image[1*(yy*p->xsize + xx) + 0];
	q->image[3*(y*q->xsize + x) + 2] = p->image[1*(yy*p->xsize + xx) + 0];
	break;
      case C_IMAGE_RGBA:
	q->image[3*(y*q->xsize + x) + 0] = p->image[4*(yy*p->xsize + xx) + 0];
	q->image[3*(y*q->xsize + x) + 1] = p->image[4*(yy*p->xsize + xx) + 1];
	q->image[3*(y*q->xsize + x) + 2] = p->image[4*(yy*p->xsize + xx) + 2];
	break;
      }
    }
  }

}
#endif

#if 0
// XXXX Does not work for all image type combinations. Fix it.
// Keeps the aspect. Fits the source image inside.
void image_scale(void *pp, void *qq)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  float xscale;
  float yscale;
  float scale;
  int xoffset;
  int yoffset;

  p = pp;
  q = qq;

  xscale = (float)p->xsize/(float)q->xsize;
  yscale = (float)p->ysize/(float)q->ysize;
  if (xscale < 1.0) xscale = 1.0; // else it is ok
  if (yscale < 1.0) yscale = 1.0; // else it is ok
  if (xscale > yscale) scale = xscale;
  else scale = yscale;

  xoffset = (int)(((float)q->xsize*scale - (float)p->xsize)/2.0);
  yoffset = (int)(((float)q->ysize*scale - (float)p->ysize)/2.0);

  for (y = 0; y < q->ysize; y++) {
    for (x = 0; x < q->xsize; x++) {
      xx = (int)((float)x*scale) - xoffset;
      yy = (int)((float)y*scale) - yoffset;
#if 0
      if (xx < 0) xx = 0;
      if (xx >= p->xsize) xx = p->xsize - 1;
      if (yy < 0) yy = 0;
      if (yy >= p->ysize) yy = p->ysize - 1;
#endif

      if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	switch (p->type) {
	case C_IMAGE_RGB:
	  q->image[3*(y*q->xsize + x) + 0] = 0;
	  q->image[3*(y*q->xsize + x) + 1] = 0;
	  q->image[3*(y*q->xsize + x) + 2] = 0;
	  break;
	case C_IMAGE_G:
	  q->image[3*(y*q->xsize + x) + 0] = 0;
	  q->image[3*(y*q->xsize + x) + 1] = 0;
	  q->image[3*(y*q->xsize + x) + 2] = 0;
	  break;
	case C_IMAGE_RGBA:
	  q->image[3*(y*q->xsize + x) + 0] = 0;
	  q->image[3*(y*q->xsize + x) + 1] = 0;
	  q->image[3*(y*q->xsize + x) + 2] = 0;
	  break;
	}
      } else {
	switch (p->type) {
	case C_IMAGE_RGB:
	  q->image[3*(y*q->xsize + x) + 0] = p->image[3*(yy*p->xsize + xx) + 0];
	  q->image[3*(y*q->xsize + x) + 1] = p->image[3*(yy*p->xsize + xx) + 1];
	  q->image[3*(y*q->xsize + x) + 2] = p->image[3*(yy*p->xsize + xx) + 2];
	  break;
	case C_IMAGE_G:
	  q->image[3*(y*q->xsize + x) + 0] = p->image[1*(yy*p->xsize + xx) + 0];
	  q->image[3*(y*q->xsize + x) + 1] = p->image[1*(yy*p->xsize + xx) + 0];
	  q->image[3*(y*q->xsize + x) + 2] = p->image[1*(yy*p->xsize + xx) + 0];
	  break;
	case C_IMAGE_RGBA:
	  q->image[3*(y*q->xsize + x) + 0] = p->image[4*(yy*p->xsize + xx) + 0];
	  q->image[3*(y*q->xsize + x) + 1] = p->image[4*(yy*p->xsize + xx) + 1];
	  q->image[3*(y*q->xsize + x) + 2] = p->image[4*(yy*p->xsize + xx) + 2];
	  break;
	}
      }
    }
  }
}
#endif

#if 0
// This version about 2.6 sec in a test.

// With zoom and center.
// XXXX Does not work for all image type combinations. Fix it.
// Keeps the aspect. Fits the source image inside.
void image_scale_zoom(void *pp, void *qq, float scale, float xcenter, float ycenter)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  // float xscale;
  // float yscale;
  // float scale;
  int xoffset;
  int yoffset;

  p = pp;
  q = qq;

#if 0
  xscale = (float)p->xsize/(float)q->xsize;
  yscale = (float)p->ysize/(float)q->ysize;
  if (xscale < 1.0) xscale = 1.0; // else it is ok
  if (yscale < 1.0) yscale = 1.0; // else it is ok
  if (xscale > yscale) scale = xscale;
  else scale = yscale;

  scale *= zoomscale;

  xoffset = (int)(((float)q->xsize*scale - (float)p->xsize)/2.0);
  yoffset = (int)(((float)q->ysize*scale - (float)p->ysize)/2.0);
  xoffset = 0;
  yoffset = 0;
#endif

  for (y = 0; y < q->ysize; y++) {
    for (x = 0; x < q->xsize; x++) {
      xx = (int)(((float)x - (float)q->xsize/2.0)*scale + (float)xcenter);
      yy = (int)(((float)y - (float)q->ysize/2.0)*scale + (float)ycenter);
#if 0
      if (xx < 0) xx = 0;
      if (xx >= p->xsize) xx = p->xsize - 1;
      if (yy < 0) yy = 0;
      if (yy >= p->ysize) yy = p->ysize - 1;
#endif

      if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	switch (p->type) {
	case C_IMAGE_RGB:
	  q->image[3*(y*q->xsize + x) + 0] = 0;
	  q->image[3*(y*q->xsize + x) + 1] = 0;
	  q->image[3*(y*q->xsize + x) + 2] = 0;
	  break;
	case C_IMAGE_G:
	  q->image[3*(y*q->xsize + x) + 0] = 0;
	  q->image[3*(y*q->xsize + x) + 1] = 0;
	  q->image[3*(y*q->xsize + x) + 2] = 0;
	  break;
	case C_IMAGE_RGBA:
	  q->image[3*(y*q->xsize + x) + 0] = 0;
	  q->image[3*(y*q->xsize + x) + 1] = 0;
	  q->image[3*(y*q->xsize + x) + 2] = 0;
	  break;
	}
      } else {
	switch (p->type) {
	case C_IMAGE_RGB:
	  q->image[3*(y*q->xsize + x) + 0] = p->image[3*(yy*p->xsize + xx) + 0];
	  q->image[3*(y*q->xsize + x) + 1] = p->image[3*(yy*p->xsize + xx) + 1];
	  q->image[3*(y*q->xsize + x) + 2] = p->image[3*(yy*p->xsize + xx) + 2];
	  break;
	case C_IMAGE_G:
	  q->image[3*(y*q->xsize + x) + 0] = p->image[1*(yy*p->xsize + xx) + 0];
	  q->image[3*(y*q->xsize + x) + 1] = p->image[1*(yy*p->xsize + xx) + 0];
	  q->image[3*(y*q->xsize + x) + 2] = p->image[1*(yy*p->xsize + xx) + 0];
	  break;
	case C_IMAGE_RGBA:
	  q->image[3*(y*q->xsize + x) + 0] = p->image[4*(yy*p->xsize + xx) + 0];
	  q->image[3*(y*q->xsize + x) + 1] = p->image[4*(yy*p->xsize + xx) + 1];
	  q->image[3*(y*q->xsize + x) + 2] = p->image[4*(yy*p->xsize + xx) + 2];
	  break;
	}
      }
    }
  }
}
#endif



// An optimized version.
// This version about 0.65 sec in a test.

// With zoom and center.
// XXXX Does not work for all image type combinations. Fix it.
// Keeps the aspect. Fits the source image inside.
void image_scale_zoom(void *pp, void *qq, float scale, float xcenter, float ycenter)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;

  p = pp;
  q = qq;

  ax = -scale*(float)q->xsize/2.0 + (float)xcenter;
  ay = -scale*(float)q->ysize/2.0 + (float)ycenter;

  switch (p->itype) {
  case C_IMAGE_RGB:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 3*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + 3*xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + 3*xx + 1];
	  q->image[by + 3*x + 2] = p->image[byy + 3*xx + 2];
	}
      }
    }
    break;
  case C_IMAGE_G:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 1*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + xx + 0];
	  q->image[by + 3*x + 2] = p->image[byy + xx + 0];
	}
      }
    }
    break;
  case C_IMAGE_RGBA:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 4*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + 4*xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + 4*xx + 1];
	  q->image[by + 3*x + 2] = p->image[byy + 4*xx + 2];
	}
      }
    }
    break;
  }

}

void image_scale_zoom_add(void *pp, void *qq, float scale, float xcenter, float ycenter)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;
  int r,g,b;

  p = pp;
  q = qq;

  ax = -scale*(float)q->xsize/2.0 + (float)xcenter;
  ay = -scale*(float)q->ysize/2.0 + (float)ycenter;

  switch (p->itype) {
  case C_IMAGE_RGB:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 3*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  // q->image[by + 3*x + 0] = 0;
	  // q->image[by + 3*x + 1] = 0;
	  // q->image[by + 3*x + 2] = 0;
	} else {
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  r += p->image[byy + 3*xx + 0];
	  g += p->image[byy + 3*xx + 1];
	  b += p->image[byy + 3*xx + 2];
	  if (r > 255) r = 255;
	  if (g > 255) g = 255;
	  if (b > 255) b = 255;
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  case C_IMAGE_G:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 1*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  // q->image[by + 3*x + 0] = 0;
	  // q->image[by + 3*x + 1] = 0;
	  // q->image[by + 3*x + 2] = 0;
	} else {
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  r += p->image[byy + xx + 0];
	  g += p->image[byy + xx + 0];
	  b += p->image[byy + xx + 0];
	  if (r > 255) r = 255;
	  if (g > 255) g = 255;
	  if (b > 255) b = 255;
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  case C_IMAGE_RGBA:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 4*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  r += p->image[byy + 4*xx + 0];
	  g += p->image[byy + 4*xx + 1];
	  b += p->image[byy + 4*xx + 2];
	  if (r > 255) r = 255;
	  if (g > 255) g = 255;
	  if (b > 255) b = 255;
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  }

}


// Based on center.
void image_scale_zoom_add_area(void *pp, void *qq, float scale, float xcenter, float ycenter, int xmin, int ymin, int xmax, int ymax)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;
  int r,g,b;

  p = pp;
  q = qq;

  ax = -scale*(float)q->xsize/2.0 + (float)xcenter;
  ay = -scale*(float)q->ysize/2.0 + (float)ycenter;

  switch (p->itype) {
  case C_IMAGE_RGB:
    for (y = ymin; y <= ymax; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 3*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  // q->image[by + 3*x + 0] = 0;
	  // q->image[by + 3*x + 1] = 0;
	  // q->image[by + 3*x + 2] = 0;
	} else {
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  r += p->image[byy + 3*xx + 0];
	  g += p->image[byy + 3*xx + 1];
	  b += p->image[byy + 3*xx + 2];
	  if (r > 255) r = 255;
	  if (g > 255) g = 255;
	  if (b > 255) b = 255;
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  case C_IMAGE_G:
    for (y = ymin; y <= ymax; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 1*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  // q->image[by + 3*x + 0] = 0;
	  // q->image[by + 3*x + 1] = 0;
	  // q->image[by + 3*x + 2] = 0;
	} else {
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  r += p->image[byy + xx + 0];
	  g += p->image[byy + xx + 0];
	  b += p->image[byy + xx + 0];
	  if (r > 255) r = 255;
	  if (g > 255) g = 255;
	  if (b > 255) b = 255;
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  case C_IMAGE_RGBA:
    for (y = ymin; y <= ymax; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 4*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  r += p->image[byy + 4*xx + 0];
	  g += p->image[byy + 4*xx + 1];
	  b += p->image[byy + 4*xx + 2];
	  if (r > 255) r = 255;
	  if (g > 255) g = 255;
	  if (b > 255) b = 255;
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  }

}


// Based on (0,0).
void image_scale_zoom_overwrite_area_viewer(void *pp, void *qq, float xscale, float yscale, float xpos, float ypos, int xmin, int ymin, int xmax, int ymax)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  // float ax,ay;
  int by,byy;
  int r,g,b;

  p = pp;
  q = qq;

  switch (p->itype) {
  case C_IMAGE_RGB:
    for (y = ymin; y <= ymax; y++) {
      yy = (int)floorf(((float)y-ypos)/yscale);
      by = 3*y*q->xsize;
      byy = 3*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	xx = (int)floorf(((float)x-xpos)/xscale);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  // q->image[by + 3*x + 0] = 0;
	  // q->image[by + 3*x + 1] = 0;
	  // q->image[by + 3*x + 2] = 0;
	} else {
	  r = p->image[byy + 3*xx + 0];
	  g = p->image[byy + 3*xx + 1];
	  b = p->image[byy + 3*xx + 2];
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  case C_IMAGE_G:
    for (y = ymin; y <= ymax; y++) {
      // yy = (int)((float)y*scale + ay);
      yy = (int)floorf(((float)y-ypos)*yscale);
      by = 3*y*q->xsize;
      byy = 1*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	// xx = (int)((float)x*scale + ax);
	xx = (int)floorf(((float)x-xpos)*xscale);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  // q->image[by + 3*x + 0] = 0;
	  // q->image[by + 3*x + 1] = 0;
	  // q->image[by + 3*x + 2] = 0;
	} else {
	  r = p->image[byy + xx + 0];
	  g = p->image[byy + xx + 0];
	  b = p->image[byy + xx + 0];
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  case C_IMAGE_RGBA:
    for (y = ymin; y <= ymax; y++) {
      // yy = (int)((float)y*scale + ay);
      yy = (int)floorf(((float)y-ypos)*yscale);
      by = 3*y*q->xsize;
      byy = 4*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	// xx = (int)((float)x*scale + ax);
	xx = (int)floorf(((float)x-xpos)*xscale);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  // q->image[by + 3*x + 0] = 0;
	  // q->image[by + 3*x + 1] = 0;
	  // q->image[by + 3*x + 2] = 0;
	} else {
	  // XXXX Check!
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  if (r < p->image[byy + 4*xx + 0]) r = p->image[byy + 4*xx + 0];
	  if (g < p->image[byy + 4*xx + 1]) g = p->image[byy + 4*xx + 1];
	  if (b < p->image[byy + 4*xx + 2]) b = p->image[byy + 4*xx + 2];
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  }

}


// Based on (0,0).
void image_scale_zoom_max_area(void *pp, void *qq, float xscale, float yscale, float xpos, float ypos, int xmin, int ymin, int xmax, int ymax)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  // float ax,ay;
  int by,byy;
  int r,g,b;

  p = pp;
  q = qq;

  // ax = -scale*(float)q->xsize/2.0 + (float)xcenter;
  // ay = -scale*(float)q->ysize/2.0 + (float)ycenter;

  switch (p->itype) {
  case C_IMAGE_RGB:
    for (y = ymin; y <= ymax; y++) {
      yy = (int)floorf(((float)y - ypos)/yscale);
      by = 3*y*q->xsize;
      byy = 3*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	xx = (int)floorf(((float)x - xpos)/xscale);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  // q->image[by + 3*x + 0] = 0;
	  // q->image[by + 3*x + 1] = 0;
	  // q->image[by + 3*x + 2] = 0;
	} else {
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  if (r < p->image[byy + 3*xx + 0]) r = p->image[byy + 3*xx + 0];
	  if (g < p->image[byy + 3*xx + 1]) g = p->image[byy + 3*xx + 1];
	  if (b < p->image[byy + 3*xx + 2]) b = p->image[byy + 3*xx + 2];
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  case C_IMAGE_G:
    for (y = ymin; y <= ymax; y++) {
      // yy = (int)((float)y*scale + ay);
      yy = (int)floorf(((float)y-ypos)*yscale);
      by = 3*y*q->xsize;
      byy = 1*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	// xx = (int)((float)x*scale + ax);
	xx = (int)floorf(((float)x-xpos)*xscale);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  // q->image[by + 3*x + 0] = 0;
	  // q->image[by + 3*x + 1] = 0;
	  // q->image[by + 3*x + 2] = 0;
	} else {
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  if (r < p->image[byy + xx + 0]) r = p->image[byy + xx + 0];
	  if (g < p->image[byy + xx + 0]) g = p->image[byy + xx + 0];
	  if (b < p->image[byy + xx + 0]) b = p->image[byy + xx + 0];
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  case C_IMAGE_RGBA:
    for (y = ymin; y <= ymax; y++) {
      // yy = (int)((float)y*scale + ay);
      yy = (int)floorf(((float)y-ypos)*yscale);
      by = 3*y*q->xsize;
      byy = 4*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	// xx = (int)((float)x*scale + ax);
	xx = (int)floorf(((float)x-xpos)*xscale);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  if (r < p->image[byy + 4*xx + 0]) r = p->image[byy + 4*xx + 0];
	  if (g < p->image[byy + 4*xx + 1]) g = p->image[byy + 4*xx + 1];
	  if (b < p->image[byy + 4*xx + 2]) b = p->image[byy + 4*xx + 2];
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  }

}


// Based on (0,0).
void image_scale_zoom_max_area_bound(void *pp, void *qq, float xscale, float yscale, float xpos, float ypos, int xmin, int ymin, int xmax, int ymax,int bxmin, int bymin, int bxmax, int bymax)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  // float ax,ay;
  int by,byy;
  int r,g,b;

  p = pp;
  q = qq;

  // ax = -scale*(float)q->xsize/2.0 + (float)xcenter;
  // ay = -scale*(float)q->ysize/2.0 + (float)ycenter;

  switch (p->itype) {
  case C_IMAGE_RGB:
    for (y = ymin; y <= ymax; y++) {
      yy = (int)floorf(((float)y - ypos)/yscale);
      by = 3*y*q->xsize;
      byy = 3*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	xx = (int)floorf(((float)x - xpos)/xscale);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize) ||
	    (xx < bxmin) || (yy < bymin) || (xx > bxmax) || (yy > bymax)) {
	  // q->image[by + 3*x + 0] = 0;
	  // q->image[by + 3*x + 1] = 0;
	  // q->image[by + 3*x + 2] = 0;
	} else {
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  if (r < p->image[byy + 3*xx + 0]) r = p->image[byy + 3*xx + 0];
	  if (g < p->image[byy + 3*xx + 1]) g = p->image[byy + 3*xx + 1];
	  if (b < p->image[byy + 3*xx + 2]) b = p->image[byy + 3*xx + 2];
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  case C_IMAGE_G:
    for (y = ymin; y <= ymax; y++) {
      // yy = (int)((float)y*scale + ay);
      yy = (int)floorf(((float)y-ypos)*yscale);
      by = 3*y*q->xsize;
      byy = 1*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	// xx = (int)((float)x*scale + ax);
	xx = (int)floorf(((float)x-xpos)*xscale);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  // q->image[by + 3*x + 0] = 0;
	  // q->image[by + 3*x + 1] = 0;
	  // q->image[by + 3*x + 2] = 0;
	} else {
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  if (r < p->image[byy + xx + 0]) r = p->image[byy + xx + 0];
	  if (g < p->image[byy + xx + 0]) g = p->image[byy + xx + 0];
	  if (b < p->image[byy + xx + 0]) b = p->image[byy + xx + 0];
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  case C_IMAGE_RGBA:
    for (y = ymin; y <= ymax; y++) {
      // yy = (int)((float)y*scale + ay);
      yy = (int)floorf(((float)y-ypos)*yscale);
      by = 3*y*q->xsize;
      byy = 4*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	// xx = (int)((float)x*scale + ax);
	xx = (int)floorf(((float)x-xpos)*xscale);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  r = q->image[by + 3*x + 0];
	  g = q->image[by + 3*x + 1];
	  b = q->image[by + 3*x + 2];
	  if (r < p->image[byy + 4*xx + 0]) r = p->image[byy + 4*xx + 0];
	  if (g < p->image[byy + 4*xx + 1]) g = p->image[byy + 4*xx + 1];
	  if (b < p->image[byy + 4*xx + 2]) b = p->image[byy + 4*xx + 2];
	  q->image[by + 3*x + 0] = r;
	  q->image[by + 3*x + 1] = g;
	  q->image[by + 3*x + 2] = b;
	}
      }
    }
    break;
  }

}

// Based on (0,0).
void image_scale_zoom_max_area_bound_rgb2rgba(void *pp, void *qq, float xscale, float yscale, float xpos, float ypos, int xmin, int ymin, int xmax, int ymax,int bxmin, int bymin, int bxmax, int bymax)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  // float ax,ay;
  int by,byy;
  int r,g,b;

  p = pp; // RGB
  q = qq; // RGBA

  for (y = ymin; y <= ymax; y++) {
    yy = (int)floorf(((float)y - ypos)/yscale);
    by = 4*y*q->xsize;
    byy = 3*yy*p->xsize;
    for (x = xmin; x <= xmax; x++) {
      xx = (int)floorf(((float)x - xpos)/xscale);
      if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize) ||
	  (xx < bxmin) || (yy < bymin) || (xx > bxmax) || (yy > bymax)) {
	// q->image[by + 4*x + 0] = 0;
	// q->image[by + 4*x + 1] = 0;
	// q->image[by + 4*x + 2] = 0;
	// q->image[by + 4*x + 3] = 255;
      } else {
	r = q->image[by + 4*x + 0];
	g = q->image[by + 4*x + 1];
	b = q->image[by + 4*x + 2];
	if (r < p->image[byy + 3*xx + 0]) r = p->image[byy + 3*xx + 0];
	if (g < p->image[byy + 3*xx + 1]) g = p->image[byy + 3*xx + 1];
	if (b < p->image[byy + 3*xx + 2]) b = p->image[byy + 3*xx + 2];
	q->image[by + 4*x + 0] = r;
	q->image[by + 4*x + 1] = g;
	q->image[by + 4*x + 2] = b;
	q->image[by + 4*x + 3] = 255;
      }
    }
  }

}


// With area.
// With zoom and center.
// XXXX Does not work for all image type combinations. Fix it.
// Keeps the aspect. Fits the source image inside.
void image_scale_zoom_area(void *pp, void *qq, float scale, float xcenter, float ycenter, int xmin, int ymin, int xmax, int ymax)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;

  p = pp;
  q = qq;

  ax = -scale*(float)q->xsize/2.0 + (float)xcenter;
  ay = -scale*(float)q->ysize/2.0 + (float)ycenter;

  switch (p->itype) {
  case C_IMAGE_RGB:
    for (y = ymin; y <= ymax; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 3*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + 3*xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + 3*xx + 1];
	  q->image[by + 3*x + 2] = p->image[byy + 3*xx + 2];
	}
      }
    }
    break;
  case C_IMAGE_G:
    for (y = ymin; y <= ymax; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 1*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + xx + 0];
	  q->image[by + 3*x + 2] = p->image[byy + xx + 0];
	}
      }
    }
    break;
  case C_IMAGE_RGBA:
    for (y = ymin; y <= ymax; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 4*yy*p->xsize;
      for (x = xmin; x <= xmax; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + 4*xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + 4*xx + 1];
	  q->image[by + 3*x + 2] = p->image[byy + 4*xx + 2];
	}
      }
    }
    break;
  }



#if 0
  for (y = ymin; y <= ymax; y++) {
    for (x = xmin; x <= xmax; x++) {
      xx = (int)(((float)x - (float)q->xsize/2.0)*scale + (float)xcenter);
      yy = (int)(((float)y - (float)q->ysize/2.0)*scale + (float)ycenter);

      if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	switch (p->itype) {
	case C_IMAGE_RGB:
	  q->image[3*(y*q->xsize + x) + 0] = 0;
	  q->image[3*(y*q->xsize + x) + 1] = 0;
	  q->image[3*(y*q->xsize + x) + 2] = 0;
	  break;
	case C_IMAGE_G:
	  q->image[3*(y*q->xsize + x) + 0] = 0;
	  q->image[3*(y*q->xsize + x) + 1] = 0;
	  q->image[3*(y*q->xsize + x) + 2] = 0;
	  break;
	case C_IMAGE_RGBA:
	  q->image[3*(y*q->xsize + x) + 0] = 0;
	  q->image[3*(y*q->xsize + x) + 1] = 0;
	  q->image[3*(y*q->xsize + x) + 2] = 0;
	  break;
	}
      } else {
	switch (p->type) {
	case C_IMAGE_RGB:
	  q->image[3*(y*q->xsize + x) + 0] = p->image[3*(yy*p->xsize + xx) + 0];
	  q->image[3*(y*q->xsize + x) + 1] = p->image[3*(yy*p->xsize + xx) + 1];
	  q->image[3*(y*q->xsize + x) + 2] = p->image[3*(yy*p->xsize + xx) + 2];
	  break;
	case C_IMAGE_G:
	  q->image[3*(y*q->xsize + x) + 0] = p->image[1*(yy*p->xsize + xx) + 0];
	  q->image[3*(y*q->xsize + x) + 1] = p->image[1*(yy*p->xsize + xx) + 0];
	  q->image[3*(y*q->xsize + x) + 2] = p->image[1*(yy*p->xsize + xx) + 0];
	  break;
	case C_IMAGE_RGBA:
	  q->image[3*(y*q->xsize + x) + 0] = p->image[4*(yy*p->xsize + xx) + 0];
	  q->image[3*(y*q->xsize + x) + 1] = p->image[4*(yy*p->xsize + xx) + 1];
	  q->image[3*(y*q->xsize + x) + 2] = p->image[4*(yy*p->xsize + xx) + 2];
	  break;
	}
      }
    }
  }
#endif

}

#if 0
// XXXX CONTINUE
// Images must be of the same size.
void image_copy(void *pp, void *qq)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;

  p = pp;
  q = qq;

  for (y = ymin; y <= ymax; y++) {
    for (x = xmin; x <= xmax; x++) {
      xx = destxmin + x - xmin;
      yy = destymin + y - ymin;

      switch (p->itype) {
      case C_IMAGE_RGB:
	q->image[3*(yy*q->xsize + xx) + 0] = p->image[3*(y*p->xsize + x) + 0];
	q->image[3*(yy*q->xsize + xx) + 1] = p->image[3*(y*p->xsize + x) + 1];
	q->image[3*(yy*q->xsize + xx) + 2] = p->image[3*(y*p->xsize + x) + 2];
	break;
#if 0
      case C_IMAGE_G:
	q->image[3*(y*q->xsize + x) + 0] = p->image[1*(y*p->xsize + x) + 0];
	q->image[3*(y*q->xsize + x) + 1] = p->image[1*(y*p->xsize + x) + 0];
	q->image[3*(y*q->xsize + x) + 2] = p->image[1*(y*p->xsize + x) + 0];
	break;
      case C_IMAGE_RGBA:
	q->image[3*(y*q->xsize + x) + 0] = p->image[4*(y*p->xsize + x) + 0];
	q->image[3*(y*q->xsize + x) + 1] = p->image[4*(y*p->xsize + x) + 1];
	q->image[3*(y*q->xsize + x) + 2] = p->image[4*(y*p->xsize + x) + 2];
	break;
#endif
      }
    }
  }
}
#endif


// Images must be of the same size.
void image_copy_rgba2rgb(void *pp, void *qq)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  int a;

  p = pp; // RGBA
  q = qq; // RGB

  for (y = 0; y < p->ysize; y++) {
    for (x = 0; x < p->xsize; x++) {
      a = p->image[4*(y*p->xsize + x) + 3];
      // q = a*p + (1-a)*q
      q->image[3*(y*q->xsize + x) + 0] = (unsigned char)((a*(int)p->image[4*(y*p->xsize + x) + 0] + (255-a)*(int)q->image[3*(y*q->xsize + x) + 0])/255);
      q->image[3*(y*q->xsize + x) + 1] = (unsigned char)((a*(int)p->image[4*(y*p->xsize + x) + 1] + (255-a)*(int)q->image[3*(y*q->xsize + x) + 1])/255);
      q->image[3*(y*q->xsize + x) + 2] = (unsigned char)((a*(int)p->image[4*(y*p->xsize + x) + 2] + (255-a)*(int)q->image[3*(y*q->xsize + x) + 2])/255);
    }
  }
}


void image_copy_rgba2rgba_alpha(void *pp, void *qq)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  int a;

  p = pp; // RGBA
  q = qq; // RGBA

  for (y = 0; y < p->ysize; y++) {
    for (x = 0; x < p->xsize; x++) {
      a = p->image[4*(y*p->xsize + x) + 3];
      // q = a*p + (1-a)*q
      q->image[4*(y*q->xsize + x) + 0] = (unsigned char)((a*(int)p->image[4*(y*p->xsize + x) + 0] + (255-a)*(int)q->image[4*(y*q->xsize + x) + 0])/255);
      q->image[4*(y*q->xsize + x) + 1] = (unsigned char)((a*(int)p->image[4*(y*p->xsize + x) + 1] + (255-a)*(int)q->image[4*(y*q->xsize + x) + 1])/255);
      q->image[4*(y*q->xsize + x) + 2] = (unsigned char)((a*(int)p->image[4*(y*p->xsize + x) + 2] + (255-a)*(int)q->image[4*(y*q->xsize + x) + 2])/255);
      q->image[4*(y*q->xsize + x) + 3] = (unsigned char)((a*(int)p->image[4*(y*p->xsize + x) + 3] + (255-a)*(int)q->image[4*(y*q->xsize + x) + 3])/255);
    }
  }
}



void image_copy_rgba2rgba_alpha_opacity(void *pp, void *qq, int opacity)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  int a;

  p = pp; // RGBA
  q = qq; // RGBA

  for (y = 0; y < p->ysize; y++) {
    for (x = 0; x < p->xsize; x++) {
      a = p->image[4*(y*p->xsize + x) + 3]*opacity/255;
      // q = a*p + (1-a)*q
      q->image[4*(y*q->xsize + x) + 0] = (unsigned char)((a*(int)p->image[4*(y*p->xsize + x) + 0] + (255-a)*(int)q->image[4*(y*q->xsize + x) + 0])/255);
      q->image[4*(y*q->xsize + x) + 1] = (unsigned char)((a*(int)p->image[4*(y*p->xsize + x) + 1] + (255-a)*(int)q->image[4*(y*q->xsize + x) + 1])/255);
      q->image[4*(y*q->xsize + x) + 2] = (unsigned char)((a*(int)p->image[4*(y*p->xsize + x) + 2] + (255-a)*(int)q->image[4*(y*q->xsize + x) + 2])/255);
      q->image[4*(y*q->xsize + x) + 3] = (unsigned char)((a*(int)p->image[4*(y*p->xsize + x) + 3] + (255-a)*(int)q->image[4*(y*q->xsize + x) + 3])/255);
    }
  }
}



void image_copy_rectangle(void *pp, int xmin, int ymin, int xmax, int ymax, void *qq, int destxmin, int destymin)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;

  p = pp;
  q = qq;

  for (y = ymin; y <= ymax; y++) {
    for (x = xmin; x <= xmax; x++) {
      xx = destxmin + x - xmin;
      yy = destymin + y - ymin;

      switch (p->itype) {
      case C_IMAGE_RGB:
	q->image[3*(yy*q->xsize + xx) + 0] = p->image[3*(y*p->xsize + x) + 0];
	q->image[3*(yy*q->xsize + xx) + 1] = p->image[3*(y*p->xsize + x) + 1];
	q->image[3*(yy*q->xsize + xx) + 2] = p->image[3*(y*p->xsize + x) + 2];
	break;
#if 0
      case C_IMAGE_G:
	q->image[3*(y*q->xsize + x) + 0] = p->image[1*(y*p->xsize + x) + 0];
	q->image[3*(y*q->xsize + x) + 1] = p->image[1*(y*p->xsize + x) + 0];
	q->image[3*(y*q->xsize + x) + 2] = p->image[1*(y*p->xsize + x) + 0];
	break;
      case C_IMAGE_RGBA:
	q->image[3*(y*q->xsize + x) + 0] = p->image[4*(y*p->xsize + x) + 0];
	q->image[3*(y*q->xsize + x) + 1] = p->image[4*(y*p->xsize + x) + 1];
	q->image[3*(y*q->xsize + x) + 2] = p->image[4*(y*p->xsize + x) + 2];
	break;
#endif
      }
    }
  }
}

#if 0
void image_clean(void *pp)
{
  ty_image *p;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;

  p = pp;

  switch (p->itype) {
  case C_IMAGE_RGB:
    for (y = 0; y < p->ysize; y++) {
      by = 3*y*p->xsize;
      for (x = 0; x < p->xsize; x++) {
	p->image[by + 3*x + 0] = 0;
	p->image[by + 3*x + 1] = 0;
	p->image[by + 3*x + 2] = 0;
      }
    }
    break;
  case C_IMAGE_G:
    for (y = 0; y < p->ysize; y++) {
      by = y*p->xsize;
      for (x = 0; x < p->xsize; x++) {
	p->image[by + x + 0] = 0;
      }
    }
    break;
  case C_IMAGE_RGBA:
    for (y = 0; y < p->ysize; y++) {
      by = 4*y*p->xsize;
      for (x = 0; x < p->xsize; x++) {
	p->image[by + 4*x + 0] = 0;
	p->image[by + 4*x + 1] = 0;
	p->image[by + 4*x + 2] = 0;
	p->image[by + 4*x + 3] = 255;
      }
    }
    break;
  }

}
#endif

void image_clean(void *pp)
{
  ty_image *p;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;
  int i;

  p = pp;

  switch (p->itype) {
  case C_IMAGE_RGB:
    (void)memset(p->image,0,3*p->xsize*p->ysize);
    break;
  case C_IMAGE_G:
    (void)memset(p->image,0,p->xsize*p->ysize);
    break;
  case C_IMAGE_RGBA:
    (void)memset(p->image,0,4*p->xsize*p->ysize);
    break;
  case C_IMAGE_FRGB:
    (void)memset(p->fimage,0,4*4*p->xsize*p->ysize);
    break;
  case C_IMAGE_FZ:
    // (void)memset(p->fimage,0,4*1*p->xsize*p->ysize);
    // Z values are initially FLT_MAX.
    for (i = 0; i < p->xsize*p->ysize; i++) {
      p->fimage[i] = 1.0e+20F;
    }
    break;
  }

}



#if 0
void image_clean_rgba_transparent(void *pp)
{
  ty_image *p;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;

  p = pp;

  for (y = 0; y < p->ysize; y++) {
    by = 4*y*p->xsize;
    for (x = 0; x < p->xsize; x++) {
      p->image[by + 4*x + 0] = 0;
      p->image[by + 4*x + 1] = 0;
      p->image[by + 4*x + 2] = 0;
      p->image[by + 4*x + 3] = 0;
    }
  }

}
#endif

void image_clean_rgba_transparent(void *pp)
{
  ty_image *p;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;

  p = pp;

  (void)memset(p->image,0,4*p->xsize*p->ysize);
}


void floatimage_clean(void *pp)
{
  ty_image *p;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;

  p = pp;

  switch (p->itype) {
  case C_IMAGE_FRGB:
    for (y = 0; y < p->ysize; y++) {
      by = 3*y*p->xsize;
      for (x = 0; x < p->xsize; x++) {
	p->fimage[by + 3*x + 0] = 0.0;
	p->fimage[by + 3*x + 1] = 0.0;
	p->fimage[by + 3*x + 2] = 0.0;
      }
    }
    break;
  }

}

void image_scale_1(void *pp, void *qq)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;
  float scale;

  p = pp;
  q = qq;

  // ax = -scale*(float)q->xsize/2.0 + (float)xcenter;
  // ay = -scale*(float)q->ysize/2.0 + (float)ycenter;
  ax = 0;
  ay = 0; 
  scale = (float)image_get_xsize(p)/(float)image_get_xsize(q);

  switch (p->itype) {
  case C_IMAGE_RGB:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 3*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + 3*xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + 3*xx + 1];
	  q->image[by + 3*x + 2] = p->image[byy + 3*xx + 2];
	}
      }
    }
    break;
  case C_IMAGE_G:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 1*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + xx + 0];
	  q->image[by + 3*x + 2] = p->image[byy + xx + 0];
	}
      }
    }
    break;
  case C_IMAGE_RGBA:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 4*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + 4*xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + 4*xx + 1];
	  q->image[by + 3*x + 2] = p->image[byy + 4*xx + 2];
	}
      }
    }
    break;
  }

}

void image_scale(void *pp, void *qq)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;
  float scale;
  int r,g,b;
  int xs,ys;

  p = pp;
  q = qq;

  // ax = -scale*(float)q->xsize/2.0 + (float)xcenter;
  // ay = -scale*(float)q->ysize/2.0 + (float)ycenter;
  ax = 0;
  ay = 0; 
  scale = (float)image_get_xsize(p)/(float)image_get_xsize(q);

  switch (p->itype) {
  case C_IMAGE_RGB:
    for (y = 0; y < q->ysize; y++) {
      for (x = 0; x < q->xsize; x++) {
	r = 0;
	g = 0;
	b = 0;
	for (ys = 0; ys < 4; ys++) {
	  for (xs = 0; xs < 4; xs++) {
	    yy = (int)(((float)y + (float)ys/4.0)*scale + ay);
	    by = 3*y*q->xsize;
	    byy = 3*yy*p->xsize;
	    xx = (int)(((float)x + (float)xs/4.0)*scale + ax);
	    if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	      // nothing
	    } else {
	      r += p->image[byy + 3*xx + 0];
	      g += p->image[byy + 3*xx + 1];
	      b += p->image[byy + 3*xx + 2];
	    }
	  }
	}
	q->image[by + 3*x + 0] = r/(4*4);
	q->image[by + 3*x + 1] = g/(4*4);
	q->image[by + 3*x + 2] = b/(4*4);
      }
    }
    break;
  case C_IMAGE_G:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 1*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + xx + 0];
	  q->image[by + 3*x + 2] = p->image[byy + xx + 0];
	}
      }
    }
    break;
  case C_IMAGE_RGBA:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 4*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + 4*xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + 4*xx + 1];
	  q->image[by + 3*x + 2] = p->image[byy + 4*xx + 2];
	}
      }
    }
    break;
  }

}

// XXXX something wrong; _16.ppm are wrong
void image_scale_supersampled(void *pp, void *qq)
// void image_scale(void *pp, void *qq)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;
  float scale;
  int *count;
  ty_image *fq;
  int r,g,b;

  p = pp;
  q = qq;

  // ax = -scale*(float)q->xsize/2.0 + (float)xcenter;
  // ay = -scale*(float)q->ysize/2.0 + (float)ycenter;
  ax = 0;
  ay = 0; 
  scale = (float)image_get_xsize(p)/(float)image_get_xsize(q);

  switch (p->itype) {
  case C_IMAGE_RGB:
    fq = floatimage_new_rgb(image_get_xsize(q),image_get_ysize(q));
    floatimage_clean(fq);
    count = (int *)malloc(image_get_xsize(q)*image_get_ysize(q)*sizeof(int));
    for (y = 0; y < q->ysize; y++) {
      for (x = 0; x < q->xsize; x++) {
	count[y*q->xsize + x] = 0;
      }
    }
    for (y = 0; y < p->ysize; y++) {
      yy = (int)((float)y/scale);
      by = 3*y*p->xsize;
      byy = 3*yy*q->xsize;
      for (x = 0; x < p->xsize; x++) {
	xx = (int)((float)x/scale);
	if ((xx < 0) || (yy < 0) || (xx >= q->xsize) || (yy >= q->ysize)) {
	  // Do nothing.
	} else {
	  fq->fimage[byy + 3*xx + 0] += (float)(p->image[by + 3*x + 0]);
	  fq->fimage[byy + 3*xx + 1] += (float)(p->image[by + 3*x + 1]);
	  fq->fimage[byy + 3*xx + 2] += (float)(p->image[by + 3*x + 2]);
	  count[yy*q->xsize + xx]++;
	}
      }
    }
    for (y = 0; y < q->ysize; y++) {
      by = 3*y*q->xsize;
      for (x = 0; x < q->xsize; x++) {
	if (count[y*q->xsize + x] == 0) count[y*q->xsize + x]++;
	r = (int)(fq->fimage[by + 3*x + 0]/count[y*q->xsize + x]);
	g = (int)(fq->fimage[by + 3*x + 1]/count[y*q->xsize + x]);
	b = (int)(fq->fimage[by + 3*x + 2]/count[y*q->xsize + x]);
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	q->image[by + 3*x + 0] = (unsigned char)r;
	q->image[by + 3*x + 1] = (unsigned char)g;
	q->image[by + 3*x + 2] = (unsigned char)b;
      }
    }
    free(count);
    free(fq->fimage);
    free(fq);
    break;
#if 0
  case C_IMAGE_G:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 1*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + xx + 0];
	  q->image[by + 3*x + 2] = p->image[byy + xx + 0];
	}
      }
    }
    break;
  case C_IMAGE_RGBA:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 4*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + 4*xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + 4*xx + 1];
	  q->image[by + 3*x + 2] = p->image[byy + 4*xx + 2];
	}
      }
    }
    break;
#endif
  }

}

void image_scale_sampled(void *pp, void *qq)
// void image_scale(void *pp, void *qq)
{
  ty_image *p;
  ty_image *q;
  int x,y;
  int xx;
  int yy;
  float ax,ay;
  int by,byy;
  float scale;
  int *count;
  ty_image *fq;
  int r,g,b;

  p = pp;
  q = qq;

  // ax = -scale*(float)q->xsize/2.0 + (float)xcenter;
  // ay = -scale*(float)q->ysize/2.0 + (float)ycenter;
  ax = 0;
  ay = 0; 
  scale = (float)image_get_xsize(p)/(float)image_get_xsize(q);

  switch (p->itype) {
  case C_IMAGE_RGB:
    fq = floatimage_new_rgb(image_get_xsize(q),image_get_ysize(q));
    floatimage_clean(fq);
    count = (int *)malloc(image_get_xsize(q)*image_get_ysize(q)*sizeof(int));
    for (y = 0; y < q->ysize; y++) {
      for (x = 0; x < q->xsize; x++) {
	count[y*q->xsize + x] = 0;
      }
    }
    for (y = 0; y < p->ysize; y++) {
      yy = (int)((float)y/scale);
      by = 3*y*p->xsize;
      byy = 3*yy*q->xsize;
      for (x = 0; x < p->xsize; x++) {
	xx = (int)((float)x/scale);
	if ((xx < 0) || (yy < 0) || (xx >= q->xsize) || (yy >= q->ysize)) {
	  // Do nothing.
	} else {
	  fq->fimage[byy + 3*xx + 0] += (float)(p->image[by + 3*x + 0]);
	  fq->fimage[byy + 3*xx + 1] += (float)(p->image[by + 3*x + 1]);
	  fq->fimage[byy + 3*xx + 2] += (float)(p->image[by + 3*x + 2]);
	  count[yy*q->xsize + xx]++;
	}
      }
    }
    for (y = 0; y < q->ysize; y++) {
      by = 3*y*q->xsize;
      for (x = 0; x < q->xsize; x++) {
	if (count[y*q->xsize + x] == 0) count[y*q->xsize + x]++;
	r = (int)(fq->fimage[by + 3*x + 0]/count[y*q->xsize + x]);
	g = (int)(fq->fimage[by + 3*x + 1]/count[y*q->xsize + x]);
	b = (int)(fq->fimage[by + 3*x + 2]/count[y*q->xsize + x]);
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	q->image[by + 3*x + 0] = (unsigned char)r;
	q->image[by + 3*x + 1] = (unsigned char)g;
	q->image[by + 3*x + 2] = (unsigned char)b;
      }
    }
    free(count);
    free(fq->image);
    free(fq);
    break;
#if 0
  case C_IMAGE_G:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 1*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + xx + 0];
	  q->image[by + 3*x + 2] = p->image[byy + xx + 0];
	}
      }
    }
    break;
  case C_IMAGE_RGBA:
    for (y = 0; y < q->ysize; y++) {
      yy = (int)((float)y*scale + ay);
      by = 3*y*q->xsize;
      byy = 4*yy*p->xsize;
      for (x = 0; x < q->xsize; x++) {
	xx = (int)((float)x*scale + ax);
	if ((xx < 0) || (yy < 0) || (xx >= p->xsize) || (yy >= p->ysize)) {
	  q->image[by + 3*x + 0] = 0;
	  q->image[by + 3*x + 1] = 0;
	  q->image[by + 3*x + 2] = 0;
	} else {
	  q->image[by + 3*x + 0] = p->image[byy + 4*xx + 0];
	  q->image[by + 3*x + 1] = p->image[byy + 4*xx + 1];
	  q->image[by + 3*x + 2] = p->image[byy + 4*xx + 2];
	}
      }
    }
    break;
#endif
  }

}

