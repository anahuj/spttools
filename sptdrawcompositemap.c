
// Copyright 2007 Juhana Sadeharju


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <zlib.h>

#include "mylib/filesdirs.h"
#include "mylib/mmapfile.h"
#include "mylib/buffer.h"
#include "mylib/image.h"

#if 0
// Library version.
#define parseprintf if (0) printf
#define parsefprintf if (0) fprintf
#define COMPILE_VERSION 1
#else
// Standalone development version.
#define parseprintf if (1) printf
#define parsefprintf if (1) fprintf
#define COMPILE_VERSION 0
#endif

typedef struct {
  char *ddsname;
  int size;
  int flags;
  char *fourcc;
  int bitcount;
  int rmask;
  int gmask;
  int bmask;
  int amask;
  int hasalphapixels;
  int hasalpha;
  int hasfourcc;
  int hasrgb;
  int haspaletteindexed1;
  int haspaletteindexed2;
  int haspaletteindexed4;
  int haspaletteindexed8;
  int hasluminance;
  int hasalphapremult;
  int hasnormal;
} ty_dds_pixelformat;

typedef struct {
  char *ddsname;
  int caps1;
  int caps2;
  int caps3;
  int caps4;
  int hastexture;
  int hascomplex;
  int hasmipmap;
  int hasvolume;
  int hascubemap;
  int hascubemappositivex;
  int hascubemapnegativex;
  int hascubemappositivey;
  int hascubemapnegativey;
  int hascubemappositivez;
  int hascubemapnegativez;
  int hasallfaces;
} ty_dds_caps;

typedef struct {
  char *ddsname;
} ty_ddsname;

typedef struct {
  char *fourcc;
  int size;
  int flags;
  int height;
  int width;
  int pitch;
  int depth;
  int mipmapcount;
  int reserved[11];
  ty_dds_pixelformat *pixelformat;
  ty_dds_caps *caps;
  int notused;
  int hasheight;
  int haswidth;
  int haspitch;
  int hasdepth;
  int hasmipmapcount;
  int haspixelformat;
  int hascaps;
  int haslinearsize;
  int numfaces;
  void **images;
} ty_dds;

char *global_ddsparser_dxttype_none = "NONE";

void parseprint_ddsdflags(int n)
{
  if (n & 0x00000002) parseprintf("  Height\n");
  if (n & 0x00000004) parseprintf("  Width\n");
  if (n & 0x00000008) parseprintf("  Pitch\n");
  if (n & 0x00800000) parseprintf("  Depth\n");
  if (n & 0x00020000) parseprintf("  Mipmapcount\n");
  if (n & 0x00001000) parseprintf("  Pixelformat\n");
  if (n & 0x00000001) parseprintf("  Caps\n");
  if (n & 0x00080000) parseprintf("  Linearsize\n");
}

void parseprint_ddpfflags(int n)
{
  if (n & 0x00000001) parseprintf("  Alphapixels\n");
  if (n & 0x00000002) parseprintf("  Alpha\n");
  if (n & 0x00000004) parseprintf("  Fourcc\n");
  if (n & 0x00000040) parseprintf("  RGB\n");
  if (n & 0x00000800) parseprintf("  PaletteIndexed1\n");
  if (n & 0x00001000) parseprintf("  PaletteIndexed2\n");
  if (n & 0x00000008) parseprintf("  PaletteIndexed4\n");
  if (n & 0x00000020) parseprintf("  PaletteIndexed8\n");
  if (n & 0x00020000) parseprintf("  Luminance\n");
  if (n & 0x00008000) parseprintf("  AlphaPremult\n");
  if (n & 0x80000000) parseprintf("  Normal\n");
}

void parseprint_caps1flags(int n)
{
  if (n & 0x00001000) parseprintf("  Texture\n");
  if (n & 0x00000008) parseprintf("  Complex\n");
  if (n & 0x00400000) parseprintf("  Mipmap\n");
}

void parseprint_caps2flags(int n)
{
  if (n & 0x00200000) parseprintf("  Volume\n");
  if (n & 0x00000200) parseprintf("  Cubemap\n");
  if (n & 0x00000400) parseprintf("  Cubemap+X\n");
  if (n & 0x00000800) parseprintf("  Cubemap-X\n");
  if (n & 0x00001000) parseprintf("  Cubemap+Y\n");
  if (n & 0x00002000) parseprintf("  Cubemap-Y\n");
  if (n & 0x00004000) parseprintf("  Cubemap+Z\n");
  if (n & 0x00008000) parseprintf("  Cubemap-Z\n");
  if (n & 0x0000fc00) parseprintf("  CubemapAllFaces\n");
}

void *parse_pixelformat(void *b)
{
  ty_dds_pixelformat *p;
  int n;
  char *s;

  p = (ty_dds_pixelformat *)malloc(sizeof(ty_dds_pixelformat));

  p->ddsname = strdup("PixelFormat");

  n = buffer_read_uint(b);
  parseprintf("Size %i\n",n);
  p->size = n;

  n = buffer_read_uint(b);
  parseprintf("Flags %x\n",n);
  parseprint_ddpfflags(n);
  p->flags = n;
  p->hasalphapixels     = n & 0x00000001;
  p->hasalpha           = n & 0x00000002;
  p->hasfourcc          = n & 0x00000004;
  p->hasrgb             = n & 0x00000040;
  p->haspaletteindexed1 = n & 0x00000800;
  p->haspaletteindexed2 = n & 0x00001000;
  p->haspaletteindexed4 = n & 0x00000008;
  p->haspaletteindexed8 = n & 0x00000020;
  p->hasluminance       = n & 0x00020000;
  p->hasalphapremult    = n & 0x00008000;
  p->hasnormal          = n & 0x80000000;

  if (p->hasfourcc != 0) {
    s = buffer_read_nstring(b,4);
    parseprintf("Fourcc %s\n",s);
    p->fourcc = s;
  } else {
    p->fourcc = global_ddsparser_dxttype_none;
  }

  n = buffer_read_uint(b);
  parseprintf("Bitcount %i\n",n);
  p->bitcount = n;

  n = buffer_read_uint(b);
  parseprintf("R mask %i\n",n);
  p->rmask = n;

  n = buffer_read_uint(b);
  parseprintf("G mask %i\n",n);
  p->gmask = n;

  n = buffer_read_uint(b);
  parseprintf("B mask %i\n",n);
  p->bmask = n;

  n = buffer_read_uint(b);
  parseprintf("A mask %i\n",n);
  p->amask = n;

  return (void *)p;
}

void *parse_caps(void *b)
{
  ty_dds_caps *p;
  int n;

  p = (ty_dds_caps *)malloc(sizeof(ty_dds_caps));

  p->ddsname = strdup("Caps");

  n = buffer_read_uint(b);
  parseprintf("Caps 1 %x\n",n);
  parseprint_caps1flags(n);
  p->caps1 = n;
  p->hastexture = n & 0x00001000;
  p->hascomplex = n & 0x00000008;
  p->hasmipmap  = n & 0x00400000;

  n = buffer_read_uint(b);
  parseprintf("Caps 2 %x\n",n);
  p->caps2 = n;
  p->hasvolume           = n & 0x00200000;
  p->hascubemap          = n & 0x00000200;
  p->hascubemappositivex = n & 0x00000400;
  p->hascubemapnegativex = n & 0x00000800;
  p->hascubemappositivey = n & 0x00001000;
  p->hascubemapnegativey = n & 0x00002000;
  p->hascubemappositivez = n & 0x00004000;
  p->hascubemapnegativez = n & 0x00008000;
  p->hasallfaces         = n & 0x0000fc00;

  n = buffer_read_uint(b);
  parseprintf("Caps 3 %x\n",n);
  p->caps3 = n;

  n = buffer_read_uint(b);
  parseprintf("Caps 4 %x\n",n);
  p->caps4 = n;

  return (void *)p;
}

#if 0
void *parse_face(ty_buffer *b)
{
  ty_dds_face *p;

  p = (ty_dds_face *)malloc(sizeof(ty_dds_face));

  p->ddsname = strdup("Face");

  return (void *)p;
}
#endif

#if 0
void *parse_(ty_buffer *b)
{
  ty_dds_ *p;

  p = (ty_dds_ *)malloc(sizeof(ty_dds_));

  p->ddsname = strdup("");

  return (void *)p;
}
#endif


#if 0
// dxt1 dxt3 dxt5 in the same procedure
void *dxt135_to_image(void *b, int w, int h, char *dxttype)
{
  void *p;
  int bw,bh,bx,by,tx,ty;
  int color0,color1;
  int rgb0[3],rgb1[3],trgb[3];
  unsigned int bits;
  int code;
  int a0123,a4567;
  int alpha;
  int loc;
  int k,n;
  unsigned char *image;

  p = image_new_rgba(w,h);
  image = image_get_buffer(p);

  if (strcmp(dxttype,"DXT1") == 0) CONTINUE
  bw = (w+3)/4;
  bh = (h+3)/4;
  // parsefprintf(stderr,"bw = %i, bh = %i\n",bw,bh);
  for (by = 0; by < bh; by++) {
    for (bx = 0; bx < bw; bx++) {
      // Alpha decoding
      a0123 = buffer_read_uint(b);
      a4567 = buffer_read_uint(b);
      // Each texel in the 4x4 block.
      for (ty = 0; ty < 4; ty++) {
	for (tx = 0; tx < 4; tx++) {
	  loc = 4*(4*ty + tx);
	  if (loc < 32) {
	    alpha = (a0123 >> loc) & 0xf;
	  } else {
	    loc -= 32;
	    alpha = (a4567 >> loc) & 0xf;
	  }
	  alpha = (alpha << 4) | alpha; // scaling from [0,f] to [0,ff]
	  // Write to image. A = alpha.
	  n = 4*((4*by+ty)*w + (4*bx+tx)) + 3;
	  if (n >= 4*w*h) {
	    parsefprintf(stderr,"alpha pixel out of image, %i >= %i\n",n,4*w*h);
	  }
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 3] = alpha;
	}
      }
      // Color decoding
#if 1
      color0 = buffer_read_ushort(b); // c0_lo c0_hi
      color1 = buffer_read_ushort(b); // c1_lo c1_hi
      bits = buffer_read_uint(b); // bits_0 ... bits_3
#else
      color0 = &(dxtimage[16*(by*bw + bx) + 8]); // c0_lo c0_hi
      color1 = &(dxtimage[16*(by*bw + bx) + 10]); // c1_lo c1_hi
      bits = &(dxtimage[16*(by*bw + bx) + 12]); // bits_0 ... bits_3
#endif
      rgb0[0] = (color0 >> 11) & 0x1f;
      rgb0[1] = (color0 >> 5) & 0x3f;
      rgb0[2] = (color0 >> 0) & 0x1f;
      rgb1[0] = (color1 >> 11) & 0x1f;
      rgb1[1] = (color1 >> 5) & 0x3f;
      rgb1[2] = (color1 >> 0) & 0x1f;
      rgb0[0] = (rgb0[0] << 3) | (rgb0[0] >> (5-3));
      rgb0[1] = (rgb0[1] << 2) | (rgb0[1] >> (6-2));
      rgb0[2] = (rgb0[2] << 3) | (rgb0[2] >> (5-3));
      rgb1[0] = (rgb1[0] << 3) | (rgb1[0] >> (5-3));
      rgb1[1] = (rgb1[1] << 2) | (rgb1[1] >> (6-2));
      rgb1[2] = (rgb1[2] << 3) | (rgb1[2] >> (5-3));

      // Each texel in the 4x4 block.
      for (ty = 0; ty < 4; ty++) {
	for (tx = 0; tx < 4; tx++) {
	  code = (bits >> (2*(4*ty+tx))) & 0x3;
	  for (k = 0; k < 3; k++) {
	    switch (code) {
	    case 0:
	      trgb[k] = rgb0[k];
	      break;
	    case 1:
	      trgb[k] = rgb1[k];
	      break;
	    case 2:
	      trgb[k] = (2*rgb0[k] + rgb1[k])/3;
	      break;
	    case 3:
	      trgb[k] = (rgb0[k] + 2*rgb1[k])/3;
	      break;
	    }
	  }
	  // Write to image. RGB = trgb.
	  n = 4*((4*by+ty)*w + (4*bx+tx)) + 2;
	  if (n >= 4*w*h) {
	    parsefprintf(stderr,"rgb pixel out of image, %i >= %i\n",n,4*w*h);
	  }
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 0] = trgb[0];
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 1] = trgb[1];
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 2] = trgb[2];
	}
      }
    }
  }
  return p;
}
#endif

#if 1
// dxt3 dxt5 dxt1 in separate procedures

void *dxt3_to_image(void *b, int w, int h)
{
  void *p;
  int bw,bh,bx,by,tx,ty;
  unsigned int color0,color1;
  int rgb0[3],rgb1[3],trgb[3];
  unsigned int bits;
  int code;
  unsigned int a0123,a4567;
  int alpha;
  int loc;
  int k,n;
  unsigned char *image;

  p = image_new_rgba(w,h);
  image = image_get_buffer(p);
  bw = (w+3)/4;
  bh = (h+3)/4;
  // parsefprintf(stderr,"bw = %i, bh = %i\n",bw,bh);
  for (by = 0; by < bh; by++) {
    for (bx = 0; bx < bw; bx++) {
      // Alpha decoding
      a0123 = buffer_read_uint(b); // XXXX use 64 bit int
      a4567 = buffer_read_uint(b);
      // Each texel in the 4x4 block.
      for (ty = 0; ty < 4; ty++) {
	for (tx = 0; tx < 4; tx++) {
	  loc = 4*(4*ty + tx);
	  if (loc < 32) {
	    alpha = (a0123 >> loc) & 0xf;
	  } else {
	    loc -= 32;
	    alpha = (a4567 >> loc) & 0xf;
	  }
	  alpha = (alpha << 4) | alpha; // scaling from [0,f] to [0,ff]
	  // Write to image. A = alpha.
	  n = 4*((4*by+ty)*w + (4*bx+tx)) + 3;
	  if (n >= 4*w*h) {
	    parsefprintf(stderr,"alpha pixel out of image, %i >= %i\n",n,4*w*h);
	  }
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 3] = (unsigned char)alpha;
	}
      }
      // Color decoding
#if 1
      color0 = buffer_read_ushort(b); // c0_lo c0_hi
      color1 = buffer_read_ushort(b); // c1_lo c1_hi
      bits = buffer_read_uint(b); // bits_0 ... bits_3
#else
      color0 = &(dxtimage[16*(by*bw + bx) + 8]); // c0_lo c0_hi
      color1 = &(dxtimage[16*(by*bw + bx) + 10]); // c1_lo c1_hi
      bits = &(dxtimage[16*(by*bw + bx) + 12]); // bits_0 ... bits_3
#endif
      rgb0[0] = (color0 >> 11) & 0x1f;
      rgb0[1] = (color0 >> 5) & 0x3f;
      rgb0[2] = (color0 >> 0) & 0x1f;
      rgb1[0] = (color1 >> 11) & 0x1f;
      rgb1[1] = (color1 >> 5) & 0x3f;
      rgb1[2] = (color1 >> 0) & 0x1f;
      rgb0[0] = (rgb0[0] << 3) | (rgb0[0] >> (5-3));
      rgb0[1] = (rgb0[1] << 2) | (rgb0[1] >> (6-2));
      rgb0[2] = (rgb0[2] << 3) | (rgb0[2] >> (5-3));
      rgb1[0] = (rgb1[0] << 3) | (rgb1[0] >> (5-3));
      rgb1[1] = (rgb1[1] << 2) | (rgb1[1] >> (6-2));
      rgb1[2] = (rgb1[2] << 3) | (rgb1[2] >> (5-3));
      // Each texel in the 4x4 block.
      for (ty = 0; ty < 4; ty++) {
	for (tx = 0; tx < 4; tx++) {
	  code = (bits >> (2*(4*ty+tx))) & 0x3;
	  for (k = 0; k < 3; k++) {
	    switch (code) {
	    case 0:
	      trgb[k] = rgb0[k];
	      break;
	    case 1:
	      trgb[k] = rgb1[k];
	      break;
	    case 2:
	      trgb[k] = (2*rgb0[k] + rgb1[k])/3;
	      break;
	    case 3:
	      trgb[k] = (rgb0[k] + 2*rgb1[k])/3;
	      break;
	    }
	  }
	  // Write to image. RGB = trgb.
	  n = 4*((4*by+ty)*w + (4*bx+tx)) + 2;
	  if (n >= 4*w*h) {
	    parsefprintf(stderr,"rgb pixel out of image, %i >= %i\n",n,4*w*h);
	  }
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 0] = trgb[0];
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 1] = trgb[1];
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 2] = trgb[2];
	}
      }
    }
  }
  return p;
}

// dxt3_to_image() and dxt5_to_image() differs only on how
// alpha is stored.
void *dxt5_to_image(void *b, int w, int h)
{
  void *p;
  int bw,bh,bx,by,tx,ty;
  int color0,color1;
  int rgb0[3],rgb1[3],trgb[3];
  unsigned int bits;
  int code;
  int alpha;
  int alpha0,alpha1;
  int k,n;
#if 1
  u_int32_t bits01,bits2345;
  u_int64_t lbits;
#else
  unsigned int bits01,bits2345;
  unsigned long long int lbits;
#endif
  unsigned char *image;

  p = image_new_rgba(w,h);
  image = image_get_buffer(p);
  bw = (w+3)/4;
  bh = (h+3)/4;
  // parsefprintf(stderr,"bw = %i, bh = %i\n",bw,bh);
  for (by = 0; by < bh; by++) {
    for (bx = 0; bx < bw; bx++) {
      // Alpha decoding
      alpha0 = buffer_read_ubyte(b);
      alpha1 = buffer_read_ubyte(b);
      bits01 = buffer_read_ushort(b);
      bits2345 = buffer_read_uint(b);
#if 1
      lbits = (u_int64_t)bits01 + (((u_int64_t)bits2345) << 16);
#else
      lbits = (unsigned long long int)bits01 + (((unsigned long long int)bits2345) << 16);
#endif
      // Each texel in the 4x4 block.
      for (ty = 0; ty < 4; ty++) {
	for (tx = 0; tx < 4; tx++) {
	  code = (lbits >> (3*(4*ty+tx))) & 0x7;
	  alpha = 255; // Sane default.
	  switch (code) {
	  case 0:
	    alpha = alpha0;
	    break;
	  case 1:
	    alpha = alpha1;
	    break;
	  case 2:
	    if (alpha0 > alpha1) alpha = (6*alpha0 + alpha1)/7;
	    else alpha = (4*alpha0 + alpha1)/5;
	    break;
	  case 3:
	    if (alpha0 > alpha1) alpha = (5*alpha0 + 2*alpha1)/7;
	    else alpha = (3*alpha0 + 2*alpha1)/5;
	    break;
	  case 4:
	    if (alpha0 > alpha1) alpha = (4*alpha0 + 3*alpha1)/7;
	    else alpha = (2*alpha0 + 3*alpha1)/5;
	    break;
	  case 5:
	    if (alpha0 > alpha1) alpha = (3*alpha0 + 4*alpha1)/7;
	    else alpha = (alpha0 + 4*alpha1)/5;
	    break;
	  case 6:
	    if (alpha0 > alpha1) alpha = (2*alpha0 + 5*alpha1)/7;
	    else alpha = 0;
	    break;
	  case 7:
	    if (alpha0 > alpha1) alpha = (alpha0 + 6*alpha1)/7;
	    else alpha = 255;
	    break;
	  }
	  // Write to image. A = alpha.
	  n = 4*((4*by+ty)*w + (4*bx+tx)) + 3;
	  if (n >= 4*w*h) {
	    parsefprintf(stderr,"alpha pixel out of image, %i >= %i\n",n,4*w*h);
	  }
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 3] = alpha;
	}
      }
      // Color decoding
#if 1
      color0 = buffer_read_ushort(b); // c0_lo c0_hi
      color1 = buffer_read_ushort(b); // c1_lo c1_hi
      bits = buffer_read_uint(b); // bits_0 ... bits_3
#else
      color0 = &(dxtimage[16*(by*bw + bx) + 8]); // c0_lo c0_hi
      color1 = &(dxtimage[16*(by*bw + bx) + 10]); // c1_lo c1_hi
      bits = &(dxtimage[16*(by*bw + bx) + 12]); // bits_0 ... bits_3
#endif
      rgb0[0] = (color0 >> 11) & 0x1f;
      rgb0[1] = (color0 >> 5) & 0x3f;
      rgb0[2] = (color0 >> 0) & 0x1f;
      rgb1[0] = (color1 >> 11) & 0x1f;
      rgb1[1] = (color1 >> 5) & 0x3f;
      rgb1[2] = (color1 >> 0) & 0x1f;
      rgb0[0] = (rgb0[0] << 3) | (rgb0[0] >> (5-3));
      rgb0[1] = (rgb0[1] << 2) | (rgb0[1] >> (6-2));
      rgb0[2] = (rgb0[2] << 3) | (rgb0[2] >> (5-3));
      rgb1[0] = (rgb1[0] << 3) | (rgb1[0] >> (5-3));
      rgb1[1] = (rgb1[1] << 2) | (rgb1[1] >> (6-2));
      rgb1[2] = (rgb1[2] << 3) | (rgb1[2] >> (5-3));
      // Each texel in the 4x4 block.
      for (ty = 0; ty < 4; ty++) {
	for (tx = 0; tx < 4; tx++) {
	  code = (bits >> (2*(4*ty+tx))) & 0x3;
	  for (k = 0; k < 3; k++) {
	    switch (code) {
	    case 0:
	      trgb[k] = rgb0[k];
	      break;
	    case 1:
	      trgb[k] = rgb1[k];
	      break;
	    case 2:
	      trgb[k] = (2*rgb0[k] + rgb1[k])/3;
	      break;
	    case 3:
	      trgb[k] = (rgb0[k] + 2*rgb1[k])/3;
	      break;
	    }
	  }
	  // Write to image. RGB = trgb.
	  n = 4*((4*by+ty)*w + (4*bx+tx)) + 2;
	  if (n >= 4*w*h) {
	    parsefprintf(stderr,"rgb pixel out of image, %i >= %i\n",n,4*w*h);
	  }
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 0] = trgb[0];
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 1] = trgb[1];
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 2] = trgb[2];
	}
      }
    }
  }
  return p;
}

void *dxt1_to_image(void *b, int w, int h)
{
  void *p;
  int bw,bh,bx,by,tx,ty;
  int color0,color1;
  int rgb0[3],rgb1[3],trgb[3];
  unsigned int bits;
  int code;
  int alpha;
  int k,n;
  unsigned char *image;

  p = image_new_rgba(w,h);
  image = image_get_buffer(p);
  bw = (w+3)/4;
  bh = (h+3)/4;
  // parsefprintf(stderr,"bw = %i, bh = %i\n",bw,bh);
  for (by = 0; by < bh; by++) {
    for (bx = 0; bx < bw; bx++) {
      // Alpha decoding
      // Each texel in the 4x4 block.
      for (ty = 0; ty < 4; ty++) {
	for (tx = 0; tx < 4; tx++) {
	  alpha = 255; // Sane default.
	  // Write to image. A = alpha.
	  n = 4*((4*by+ty)*w + (4*bx+tx)) + 3;
	  if (n >= 4*w*h) {
	    parsefprintf(stderr,"alpha pixel out of image, %i >= %i\n",n,4*w*h);
	  }
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 3] = alpha;
	}
      }
      // Color decoding
#if 1
      color0 = buffer_read_ushort(b); // c0_lo c0_hi
      color1 = buffer_read_ushort(b); // c1_lo c1_hi
      bits = buffer_read_uint(b); // bits_0 ... bits_3
#else
      color0 = &(dxtimage[16*(by*bw + bx) + 8]); // c0_lo c0_hi
      color1 = &(dxtimage[16*(by*bw + bx) + 10]); // c1_lo c1_hi
      bits = &(dxtimage[16*(by*bw + bx) + 12]); // bits_0 ... bits_3
#endif
      rgb0[0] = (color0 >> 11) & 0x1f;
      rgb0[1] = (color0 >> 5) & 0x3f;
      rgb0[2] = (color0 >> 0) & 0x1f;
      rgb1[0] = (color1 >> 11) & 0x1f;
      rgb1[1] = (color1 >> 5) & 0x3f;
      rgb1[2] = (color1 >> 0) & 0x1f;
      rgb0[0] = (rgb0[0] << 3) | (rgb0[0] >> (5-3));
      rgb0[1] = (rgb0[1] << 2) | (rgb0[1] >> (6-2));
      rgb0[2] = (rgb0[2] << 3) | (rgb0[2] >> (5-3));
      rgb1[0] = (rgb1[0] << 3) | (rgb1[0] >> (5-3));
      rgb1[1] = (rgb1[1] << 2) | (rgb1[1] >> (6-2));
      rgb1[2] = (rgb1[2] << 3) | (rgb1[2] >> (5-3));

      // Each texel in the 4x4 block.
      for (ty = 0; ty < 4; ty++) {
	for (tx = 0; tx < 4; tx++) {
	  code = (bits >> (2*(4*ty+tx))) & 0x3;
	  for (k = 0; k < 3; k++) {
	    switch (code) {
	    case 0:
	      trgb[k] = rgb0[k];
	      break;
	    case 1:
	      trgb[k] = rgb1[k];
	      break;
	    case 2:
	      trgb[k] = (2*rgb0[k] + rgb1[k])/3;
	      break;
	    case 3:
	      trgb[k] = (rgb0[k] + 2*rgb1[k])/3;
	      break;
	    }
	  }
	  // Write to image. RGB = trgb.
	  n = 4*((4*by+ty)*w + (4*bx+tx)) + 2;
	  if (n >= 4*w*h) {
	    parsefprintf(stderr,"rgb pixel out of image, %i >= %i\n",n,4*w*h);
	  }
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 0] = trgb[0];
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 1] = trgb[1];
	  image[4*((4*by+ty)*w + (4*bx+tx)) + 2] = trgb[2];
	}
      }
    }
  }
  return p;
}

// dxt3 dxt5 dxt1
#endif

char global_stmp[1000];

void *parse_dds(void *b)
{
  int i,n;
  char *s;
  ty_dds *p;
  int blocksize;
  int j,w,h;

  p = (ty_dds *)malloc(sizeof(ty_dds));

  // Read the header string.
  s = buffer_read_nstring(b,4);
  if (strcmp(s,"DDS ") != 0) {
    fprintf(stderr,"Not a DDS file\n");
    free(s);
    return NULL;
  }
  p->fourcc = s;

  n = buffer_read_uint(b);
  parseprintf("Size %i\n",n);
  p->size = n;

  n = buffer_read_uint(b);
  parseprintf("Flags %x\n",n);
  parseprint_ddsdflags(n);
  p->flags = n;
  p->hasheight = n & 0x00000002; 
  p->haswidth = n & 0x00000004;
  p->haspitch = n & 0x00000008;
  p->hasdepth = n & 0x00800000;
  p->hasmipmapcount = n & 0x00020000;
  p->haspixelformat = n & 0x00001000;
  p->hascaps = n & 0x00000001;
  p->haslinearsize = n & 0x00080000;

  n = buffer_read_uint(b);
  parseprintf("Height %i\n",n);
  p->height = n;

  n = buffer_read_uint(b);
  parseprintf("Width %i\n",n);
  p->width = n;

  n = buffer_read_uint(b);
  parseprintf("Pitch %i\n",n);
  p->pitch = n;

  n = buffer_read_uint(b);
  parseprintf("Depth %i\n",n);
  p->depth = n;

  n = buffer_read_uint(b);
  parseprintf("Mipmap count %i\n",n);
  p->mipmapcount = n;

  parseprintf("Reserved[11]:\n");
  for (i = 0; i < 11; i++) {
    n = buffer_read_uint(b);
    parseprintf("%i\n",n);
    p->reserved[i] = n;
  }

  p->pixelformat = parse_pixelformat(b);

  if (p->pixelformat->hasfourcc != 0) {
    if ((strcmp(p->pixelformat->fourcc,"DXT3") != 0) &&
	(strcmp(p->pixelformat->fourcc,"DXT5") != 0) &&
	(strcmp(p->pixelformat->fourcc,"DXT1") != 0)) {
      exit(-1);
    }
  } else if ((p->pixelformat->hasrgb != 0) ||
	     (p->pixelformat->hasalphapixels != 0)) {
    // Do nothing.
    // XXXX exit for now
    exit(-1);
  } else {
    exit(-1);
  }

  if (p->pixelformat->hasfourcc != 0) {
    if (p->pixelformat->fourcc[3] == '1') blocksize = 8;
    else blocksize = 16; // DXT2-5 RXGB ATI2
  } else blocksize = -1;

  p->caps = parse_caps(b);

  if (p->caps->hastexture != 0) p->numfaces = 1;
  else if (p->caps->hascubemap != 0) p->numfaces = 6;
  // else p->numfaces = -1;
  else p->numfaces = 1;

  n = buffer_read_uint(b);
  parseprintf("Not used %i\n",n);
  p->notused = n;

  parsefprintf(stderr,"numfaces = %i\n",p->numfaces);
  parsefprintf(stderr,"mipmapcount = %i\n",p->mipmapcount);
  // fprintf(stderr,"numfaces = %i\n",p->numfaces);
  // fprintf(stderr,"mipmapcount = %i\n",p->mipmapcount);
  // XXXX dirty fix, faces = 1, mipmapcount = 0
  if (p->mipmapcount == 0) p->mipmapcount = 1;

  if ((p->numfaces > 0) && (p->mipmapcount > 0)) {
    p->images =
      (void **)malloc(p->numfaces*p->mipmapcount*sizeof(void *));
    for (i = 0; i < p->numfaces; i++) {
      w = p->width;
      h = p->height;
      for (j = 0; j < p->mipmapcount; j++) {
	// XXXX buffer_read_nbytes() with one copy.
	// XXXX because we have mmapped the file, could we
	// just give the ptr to dxt3_to_image?
	// dxtimage = buffer_read_nstring(b,w*h);
	// p->images[i*p->mipmapcount + j] = dxt3_to_ddsimage(dxtimage,w,h);
	if ((w >= 4) && (h >= 4)) {
	  parsefprintf(stderr,"w = %i, h = %i\n",w,h);
	  if (strcmp(p->pixelformat->fourcc,"DXT3") == 0) {
	    p->images[i*p->mipmapcount + j] = dxt3_to_image(b,w,h);
	  } else if (strcmp(p->pixelformat->fourcc,"DXT5") == 0) {
	    p->images[i*p->mipmapcount + j] = dxt5_to_image(b,w,h);
	  } else if (strcmp(p->pixelformat->fourcc,"DXT1") == 0) {
	    p->images[i*p->mipmapcount + j] = dxt1_to_image(b,w,h);
	  }
	  // free(dxtimage);
	} else {
	  // Skip the image.
	  buffer_skip_bytes(b,w*h);
	  p->images[i*p->mipmapcount + j] = NULL;
	}
	w = w >> 1;
	h = h >> 1;
	// d >> 1; if (d == 0) d = 1;
      }
    }
  }

  return (void *)p;
}

char *dds_get_ddsname(void *p)
{
  ty_ddsname *q;

  q = p;
  return q->ddsname;
}


void *dds_get_image(void *p, int face, int mipmap)
{
  ty_dds *q;

  q = p;
  return q->images[face*q->mipmapcount + mipmap];
}


#if COMPILE_VERSION
// Library version.

void *dds_read(char *s)
{
  void *dds;
  void *m;
  unsigned char *buffer;
  int buflen;
  void *b;

  m = mmapfile_open_r(s);
  if (m == NULL) return NULL;

  buffer = mmapfile_getbuf(m);
  buflen = mmapfile_getsize(m);

  b = buffer_new(buffer,buflen);

#if 0
  global_locn = 0;
  global_loc[0] = 0;
#endif

  dds = parse_dds(b);

  buffer_free(b);
  mmapfile_close(m);

  return dds;
}

#else
// Standalone version.

int main(int ac, char **av)
{
  void *dds;
  void *m;
  unsigned char *buffer;
  int buflen;
  void *b;
  ty_dds *p;
  int i,j;
  void *image;

  if (ac < 1) {
    fprintf(stderr,"Usage: sptdrawcompositemap inputimage > outputimage\n");
    exit(-1);
  }

  image = image_load_file(av[1]);

  image_save_stdout(image);
  image_free(image);

  return 1;
}
#endif
