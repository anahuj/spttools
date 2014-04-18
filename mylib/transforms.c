
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


// Multiple representation of the same transformation.
typedef struct {
  int type;
  float *matrices;
  float *pLW;
  float *pWL;
  float *nLW;
  float *nWL;
} ty_transforms_matrix;

typedef struct {
  int type;
  int levels;
  char **data;
  int datasize;
  int level;
} ty_transforms_stack;

typedef struct {
  char *name;
  ty_transforms_stack *stack;
  ty_transforms_matrix *matrix; // Current transformation.
} ty_transforms_name;

typedef struct {
  int nummaxnames;
  int numnames;
  ty_transforms_name **names;
  int activename;
  int namestack[128];
  int namestacklevel;
} ty_transforms;

ty_transforms *global_transforms;

float *pLW,*pWL;
float *nLW,*nWL;

#if 0
typedef struct {
  int type;
  float da,db;
  int xres,yres;
  float fov; // XXXX 
  float *persmatrix;
  float *posmatrix;
  float *invposmatrix;
  float *pos;
  float xpos[3];
  float ypos[3];
  float zpos[3];
} ty_camera;
#endif

#define PI 3.14159265358979323846
#define EPS 0.000001


// XXXX this is a bad idea; use rarely
float *vector3_new(float x, float y, float z)
{
  float *p;

  p = (float *)malloc(3*sizeof(float));
  p[0] = x;
  p[1] = y;
  p[2] = z;
  return p;
}

// XXXX p,x,y,z order?
void vector3_set(float x, float y, float z, float *p)
{
  p[0] = x;
  p[1] = y;
  p[2] = z;
}

float vector3_maxabscoordval(float *p)
{
  float max,v;

  max = fabs(p[0]);
  v = fabs(p[1]);
  if (max < v) max = v;
  v = fabs(p[2]);
  if (max < v) max = v;
  return max;
}

float vector3_dis2(float *p)
{
  return p[0]*p[0] + p[1]*p[1] + p[2]*p[2];
}

/* if the vector is very small and has no direction, the check is done
 * in the caller;
 * can be applied on place;
 */
void vector3_unite(float *x, float *y)
{
  float r;

  r = x[0]*x[0] + x[1]*x[1] + x[2]*x[2];
  r = 1.0/sqrt(r);
  y[0] = x[0]*r;
  y[1] = x[1]*r;
  y[2] = x[2]*r;
}

float vector3_length(float *p)
{
  return sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
}

// XXXX z,x,y order? z = x + y
// in C++, we could have z = x + y, but for some other operation
// we could have a procedure call; proc(z,x,y)
void vector3_add(float *x, float *y, float *z)
{
  z[0] = x[0] + y[0];
  z[1] = x[1] + y[1];
  z[2] = x[2] + y[2];
}

float vector3_dotprod(float *x, float *y)
{
  return x[0]*y[0] + x[1]*y[1] + x[2]*y[2];
}

void vector3_crossprod(float *x, float *y, float *z)
{
  z[0] = x[1]*y[2] - x[2]*y[1];
  z[1] = x[2]*y[0] - x[0]*y[2];
  z[2] = x[0]*y[1] - x[1]*y[0];
}

void vector3_negate(float *x, float *y)
{
  y[0] = -x[0];
  y[1] = -x[1];
  y[2] = -x[2];
}

void vector3_copy(float *x, float *y)
{
  y[0] = x[0];
  y[1] = x[1];
  y[2] = x[2];
}

/* returns y or -y so that x.y > 0 */
void vector3_ff(float *x, float *y, float *z)
{
  if (x[0]*y[0] + x[1]*y[1] + x[2]*y[2] < 0.0) {
    z[0] = -y[0];
    z[1] = -y[1];
    z[2] = -y[2];
  } else {
    z[0] = y[0];
    z[1] = y[1];
    z[2] = y[2];
  }
}

void vector3_scalarmultiply(float t, float *x, float *y)
{
  y[0] = t*x[0];
  y[1] = t*x[1];
  y[2] = t*x[2];
}

float *vector4_new(float x, float y, float z, float w)
{
  float *p;

  p = (float *)malloc(4*sizeof(float));
  p[0] = x;
  p[1] = y;
  p[2] = z;
  p[3] = z;
  return p;
}

void vector4_copy(float *x, float *y)
{
  int i;

  for(i = 0; i < 4; i++) y[i] = x[i];
}

void matrix4_copy(float *x, float *y)
{
  int i;

  for(i = 0; i < 16; i++) y[i] = x[i];
}


/* stack; data can be copied directly to the stack;
 * use this for render time transform stack;
 */
ty_transforms_stack *stack_new_fixed(int n, int datasize)
{
  ty_transforms_stack *p;
  int i;

  p = (ty_transforms_stack *)malloc(sizeof(ty_transforms_stack));
  p->levels = n;
  p->data = (char **)malloc(n*sizeof(char *));
  p->data[0] = (char *)malloc(n*datasize*sizeof(char));
  for(i = 1; i < n; i++) p->data[i] = p->data[0] + i*datasize;
  p->datasize = datasize;
  p->level = 0;
  return p;
}

void stack_push_fixed(ty_transforms_stack *p, char *data)
{
  int i;

  for(i = 0; i < p->datasize; i++) {
    p->data[p->level][i] = data[i];
  }
  p->level++;
}

void stack_pop_fixed(ty_transforms_stack *p, char *data)
{
  int i;

  p->level--;
  for(i = 0; i < p->datasize; i++) {
    data[i] = p->data[p->level][i];
  }
}


float *matrix4_new()
{
  float *p;

  p = (float *)malloc(16*sizeof(float));
  return p;
}

void matrix4_genunit(float *p)
{
  int i;

  for (i = 0; i < 16; i++) p[i] = 0.0;
  p[0] = 1.0;
  p[5] = 1.0;
  p[10] = 1.0;
  p[15] = 1.0;
}

/* keep all in one array;
 * transform matrixes are: pLW, pWL, nLW, nWL in this order;
 */
float *matrices_new()
{
  float *p;

  p = (float *)malloc(4*16*sizeof(float));
  return p;
}

void matrices_genunit(float *p)
{
  float *x;

  x = &(p[0]);
  matrix4_genunit(x);
  x = &(p[16]);
  matrix4_genunit(x);
  x = &(p[2*16]);
  matrix4_genunit(x);
  x = &(p[3*16]);
  matrix4_genunit(x);
}

#define C_TYPE_TRANSFORMS 1


ty_transforms_matrix *transforms_matrix_new()
{
  ty_transforms_matrix *p;

  p = (ty_transforms_matrix *)malloc(sizeof(ty_transforms_matrix));
  // p->type = C_TYPE_TRANSFORMS;
  p->matrices = matrices_new();
  matrices_genunit(p->matrices);
  p->pLW = &(p->matrices[0]);
  p->pWL = &(p->matrices[16]);
  p->nLW = &(p->matrices[2*16]);
  p->nWL = &(p->matrices[3*16]);

  return p;
}

float *transforms_matrix_get_pLW(ty_transforms_matrix *p)
{
  return p->pLW;
}

float *transforms_matrix_get_pWL(ty_transforms_matrix *p)
{
  return p->pWL;
}

float *transforms_matrix_get_nLW(ty_transforms_matrix *p)
{
  return p->nLW;
}

float *transforms_matrix_get_nWL(ty_transforms_matrix *p)
{
  return p->nWL;
}

void transforms_matrix_setidentity(ty_transforms_matrix *p)
{
  matrices_genunit(p->matrices);
}

void transforms_setidentity()
{
  ty_transforms *p;
  ty_transforms_name *n;
  ty_transforms_matrix *t;

  p = global_transforms;
  n = p->names[p->activename];
  t = n->matrix;
  matrices_genunit(t->matrices);
}

float *transforms_get_matrices()
{
  ty_transforms *p;
  ty_transforms_name *n;
  ty_transforms_matrix *t;

  p = global_transforms;
  n = p->names[p->activename];
  t = n->matrix;
  return t->matrices;
}

float deg2rad(float x)
{
  return(0.017453292519943*x);
  return(PI*x/180.0);
}


void matrix_mul_hM(float *x, float *m, float *y)
{
  int i,j;

  for(i = 0; i < 4; i++) y[i] = 0.0;
  for(j = 0; j < 4; j++) {
    for(i = 0; i < 4; i++) {
      y[i] += x[j]*m[4*j+i];
    }
  }
}

/* y might be x or m */
void matrix_mul_hMx(float *x, float *m, float *y)
{
  float tmp[4];

  matrix_mul_hM(x,m,tmp);
  vector4_copy(tmp,y);
}

void matrix_mul_MM(float *x, float *m, float *y)
{
  int i,j,k;
  int ip,ipj,kj;

  for(i = 0; i < 16; i++) y[i] = 0.0;
  for(i = 0; i < 4; i++) {
    ip = i<<2;
    for(j = 0; j < 4; j++) {
      ipj = ip+j;
      kj = j;
      for(k = 0; k < 4; k++) {
	y[ipj] += x[ip+k]*m[kj];
	kj += 4;
      }
    }
  }
}

// XXXX move tmp[16] to global level; the same tmp[] can be used for all

/* y might be x or m */
void matrix_mul_MMx(float *x, float *m, float *y)
{
  float tmp[16];

  matrix_mul_MM(x,m,tmp);
  matrix4_copy(tmp,y);

}

void tran(float x, float y, float z)
{
  float t[16];

  matrix4_genunit(t);
  t[12] = x;
  t[13] = y;
  t[14] = z;
  matrix_mul_MMx(t,pLW,pLW);
  t[12] = -x;
  t[13] = -y;
  t[14] = -z;
  matrix_mul_MMx(pWL,t,pWL);
}


void scal(float x, float y, float z)
{
  float t[16];

  matrix4_genunit(t);
  t[0] = x;
  t[5] = y;
  t[10] = z;
  matrix_mul_MMx(t,pLW,pLW);
  matrix_mul_MMx(nWL,t,nWL);
  t[0] = 1.0/x;
  t[5] = 1.0/y;
  t[10] = 1.0/z;
  matrix_mul_MMx(pWL,t,pWL);
  matrix_mul_MMx(t,nLW,nLW);
}

void rotx(float x)
{
  float t[16];
  float s,c;
  float a;

  a = deg2rad(x);
  s = sin(a);
  c = cos(a);
  matrix4_genunit(t);
  t[5] = c;
  t[6] = -s;
  t[9] = s;
  t[10] = c;
  matrix_mul_MMx(t,pLW,pLW);
  matrix_mul_MMx(t,nLW,nLW);
  s = -s;
  /* c = c; */
  t[5] = c;
  t[6] = -s;
  t[9] = s;
  t[10] = c;
  matrix_mul_MMx(pWL,t,pWL);
  matrix_mul_MMx(nWL,t,nWL);
}


void roty(float x)
{
  float t[16];
  float s,c;
  float a;

  a = deg2rad(x);
  s = sin(a);
  c = cos(a);
  matrix4_genunit(t);
  t[0] = c;
  t[2] = s;
  t[8] = -s;  /*  FIXME: do we have an error here?? c -s s c? or in rotx? */
  t[10] = c;
  matrix_mul_MMx(t,pLW,pLW);
  matrix_mul_MMx(t,nLW,nLW);
  s = -s;
  /* c = c; */
  t[0] = c;
  t[2] = s;
  t[8] = -s;
  t[10] = c;
  matrix_mul_MMx(pWL,t,pWL);
  matrix_mul_MMx(nWL,t,nWL);
}


void rotz(float x)
{
  float t[16];
  float s,c;
  float a;

  a = deg2rad(x);
  s = sin(a);
  c = cos(a);
  matrix4_genunit(t);
  t[0] = c;
  t[1] = -s;
  t[4] = s;
  t[5] = c;
  matrix_mul_MMx(t,pLW,pLW);
  matrix_mul_MMx(t,nLW,nLW);
  s = -s;
  /* c = c; */
  t[0] = c;
  t[1] = -s;
  t[4] = s;
  t[5] = c;
  matrix_mul_MMx(pWL,t,pWL);
  matrix_mul_MMx(nWL,t,nWL);
}

/* check this function again; the camera is just an another object in the
 * scene;
 */
void pers(float x)
{
  float t[16];
  float s,c;
  float a;

  a = deg2rad(x);
  s = sin(a/2.0);
  c = cos(a/2.0);
  matrix4_genunit(t);
  t[0] = c;
  t[5] = c;
  t[10] = s;
  t[11] = s;
  t[15] = 0.0;
  matrix_mul_MMx(t,pLW,pLW);
}

void tranrotscal(float *ptran, float *prot, float pscale)
{
  float t[16];

  matrix4_genunit(t);
  // Rot and scale here.
  t[0] = pscale * prot[0];
  t[1] = pscale * prot[3];
  t[2] = pscale * prot[6];
  t[4] = pscale * prot[1];
  t[5] = pscale * prot[4];
  t[6] = pscale * prot[7];
  t[8] = pscale * prot[2];
  t[9] = pscale * prot[5];
  t[10] = pscale * prot[8];
  matrix_mul_MMx(t,nLW,nLW);
  // Add tran here.
  t[12] = ptran[0];
  t[13] = ptran[1];
  t[14] = ptran[2];
  matrix_mul_MMx(t,pLW,pLW);

  // Inverse of 3x3 rotation R is R^T.
  // But for 4x4 matrix the translation requires extra.
  // Forward 4x4: (sR 0; 0 1)(I 0; T 1) = (sR 0; T 1)
  // Inverse 4x4: (I 0; -T 1)(1/sR^t 0; 0 1) = (1/sR^t 0; -T1/sR^t 1)
  pscale = 1.0/pscale;
  matrix4_genunit(t);
  // Rot and scale here.
  // This is the inverse without inverse translation.
  t[0] = pscale * prot[0];
  t[1] = pscale * prot[1];
  t[2] = pscale * prot[2];
  t[4] = pscale * prot[3];
  t[5] = pscale * prot[4];
  t[6] = pscale * prot[5];
  t[8] = pscale * prot[6];
  t[9] = pscale * prot[7];
  t[10] = pscale * prot[8];
  matrix_mul_MMx(nWL,t,nWL);
  // Add tran here.
  // Here we have -T 1/sR^t
  t[12] = -ptran[0]*t[0] - ptran[1]*t[4] - ptran[2]*t[8];
  t[13] = -ptran[0]*t[1] - ptran[1]*t[5] - ptran[2]*t[9];
  t[14] = -ptran[0]*t[2] - ptran[1]*t[6] - ptran[2]*t[10];
  matrix_mul_MMx(pWL,t,pWL);

}

void rot33(float *prot)
{
  float t[16];

  matrix4_genunit(t);
  // Rot and scale here.
  // This is correct order.
  t[0] = prot[0];
  t[1] = prot[3];
  t[2] = prot[6];
  t[4] = prot[1];
  t[5] = prot[4];
  t[6] = prot[7];
  t[8] = prot[2];
  t[9] = prot[5];
  t[10] = prot[8];
  matrix_mul_MMx(t,nLW,nLW);
  matrix_mul_MMx(t,pLW,pLW);

  // Inverse of 3x3 rotation R is R^T.
  matrix4_genunit(t);
  // This is the inverse.
  t[0] = prot[0];
  t[1] = prot[1];
  t[2] = prot[2];
  t[4] = prot[3];
  t[5] = prot[4];
  t[6] = prot[5];
  t[8] = prot[6];
  t[9] = prot[7];
  t[10] = prot[8];
  matrix_mul_MMx(nWL,t,nWL);
  matrix_mul_MMx(pWL,t,pWL);

}

void push()
{
  stack_push_fixed(global_transforms->names[global_transforms->activename]->stack,transforms_get_matrices());
}

void pop()
{
  stack_pop_fixed(global_transforms->names[global_transforms->activename]->stack,transforms_get_matrices());
}

// Store the current transforms as ID?
// Nope. Add a name/ID node to scenegraph. Add its path
// to the (name/ID, path) list.
void direct_pos(char *s)
{
}



void transform_p(float *x, float *m, float *y)
{
  float a[4],b[4];

  a[0] = x[0];
  a[1] = x[1];
  a[2] = x[2];
  a[3] = 1.0;
  matrix_mul_hM(a,m,b);
  y[0] = b[0]/b[3];
  y[1] = b[1]/b[3];
  y[2] = b[2]/b[3];
}

void transform_n(float *x, float *m, float *y)
{
  float a[4],b[4];

  a[0] = x[0];
  a[1] = x[1];
  a[2] = x[2];
  a[3] = 0.0;
  matrix_mul_hM(a,m,b);
  y[0] = b[0];
  y[1] = b[1];
  y[2] = b[2];
}

#if 0
void *transforms_new()
{
  ty_transforms *p;
  int i;

  p = (ty_transforms *)malloc(sizeof(ty_transforms));
  p->nummaxnames = 10;
  p->numnames = 0;
  p->names = (ty_transforms_name **)malloc(p->nummaxnames*sizeof(ty_transforms_name *));

  for (i = 0; i < p->nummaxnames; i++) {
    p->names[i] = NULL;
  }

  global_transforms = p;
  return p;
}
#endif

void transforms_init()
{
  ty_transforms *p;
  int i;

  p = (ty_transforms *)malloc(sizeof(ty_transforms));
  p->nummaxnames = 10;
  p->numnames = 0;
  p->names = (ty_transforms_name **)malloc(p->nummaxnames*sizeof(ty_transforms_name *));

  for (i = 0; i < p->nummaxnames; i++) {
    p->names[i] = NULL;
  }

  // Name stack. _use_name() / _pop_name()
  // The index in names[] is stored to stack.
  p->namestacklevel = 0; // Empty.

  global_transforms = p;
}

void transforms_use(void *p)
{
  global_transforms = p;
}

void transforms_new_name(char *s)
{
  ty_transforms *p;
  ty_transforms_name *n;

  p = global_transforms;
  if (p->numnames < p->nummaxnames) {
    n = (ty_transforms_name *)malloc(sizeof(ty_transforms_name));
    n->name = strdup(s);
    n->stack = stack_new_fixed(30,4*16*sizeof(float));
    n->matrix = transforms_matrix_new();
    p->names[p->numnames] = n;
    p->activename = p->numnames;
    p->numnames++;

    pLW = transforms_matrix_get_pLW(n->matrix);
    pWL = transforms_matrix_get_pWL(n->matrix);
    nLW = transforms_matrix_get_nLW(n->matrix);
    nWL = transforms_matrix_get_nWL(n->matrix);

  } else {
    fprintf(stderr,"transforms: no space for new name\n");
  }
}

void transforms_use_name(char *s)
{
  ty_transforms *p;
  ty_transforms_name *n;
  int i;

  p = global_transforms;

  if (p->activename > 0) {
    // XXXX Test for overflow missing.
    p->namestack[p->namestacklevel] = p->activename;
    p->namestacklevel++;
  }

  for (i = 0; i < p->numnames; i++) {
    n = p->names[i];
    if (strcmp(n->name,s) == 0) {
      p->activename = i;

      pLW = transforms_matrix_get_pLW(n->matrix);
      pWL = transforms_matrix_get_pWL(n->matrix);
      nLW = transforms_matrix_get_nLW(n->matrix);
      nWL = transforms_matrix_get_nWL(n->matrix);

      return;
    }
  }
  fprintf(stderr,"transforms_use_name: no name %s\n",s);
}

void transforms_pop_name()
{
  ty_transforms *p;
  ty_transforms_name *n;
  int i;

  p = global_transforms;

  if (p->namestacklevel > 0) {
    p->namestacklevel--;
    p->activename = p->namestack[p->namestacklevel];

    n = p->names[p->activename];
    pLW = transforms_matrix_get_pLW(n->matrix);
    pWL = transforms_matrix_get_pWL(n->matrix);
    nLW = transforms_matrix_get_nLW(n->matrix);
    nWL = transforms_matrix_get_nWL(n->matrix);

  } else {
    fprintf(stderr,"transforms_pop_name: no name to pop\n");
  }

}

float *transforms_get_pLW()
{
  return pLW;
}

float *transforms_get_pWL()
{
  return pWL;
}

float *transforms_get_nLW()
{
  return nLW;
}

float *transforms_get_nWL()
{
  return nWL;
}

#if 0
float *transforms_get_pLW()
{
  ty_transforms *p;
  ty_transforms_name *n;

  p = global_transforms;
  n = p->names[p->activename];
  return transforms_matrix_get_pLW(n->matrix);
}

float *transforms_get_pWL()
{
  ty_transforms *p;
  ty_transforms_name *n;

  p = global_transforms;
  n = p->names[p->activename];
  return transforms_matrix_get_pWL(n->matrix);
}

float *transforms_get_nLW()
{
  ty_transforms *p;
  ty_transforms_name *n;

  p = global_transforms;
  n = p->names[p->activename];
  return transforms_matrix_get_nLW(n->matrix);
}

float *transforms_get_nWL()
{
  ty_transforms *p;
  ty_transforms_name *n;

  p = global_transforms;
  n = p->names[p->activename];
  return transforms_matrix_get_nWL(n->matrix);
}
#endif


#if 0
// XXXX No camera.
// XXXX World vs. objects, and world vs. viewer.

// Return 1 if projection succeeds; otherwise 0.
int projectpoint(float *pworld, float *x, float *y, float *depth)
{
  float p1[4];

  // fprintf(stderr,"test\n");

  p1[0] = pworld[0];
  p1[1] = pworld[1];
  p1[2] = pworld[2];
  p1[3] = 1.0;
  matrix_mul_hMx(p1,global_camera->posmatrix,p1);
  if (fabs(p1[3]) < EPS) return 0;
  if (p1[2]/p1[3] < EPS) return 0;
  *depth = (p1[0]*p1[0] + p1[1]*p1[1] + p1[2]*p1[2])/(p1[3]*p1[3]);
#if 0
  *depth = sqrt((pworld[0]-global_camera->pos[0])
		*(pworld[0]-global_camera->pos[0])
		+(pworld[1]-global_camera->pos[1])
		*(pworld[1]-global_camera->pos[1])
		+(pworld[2]-global_camera->pos[2])
		*(pworld[2]-global_camera->pos[2]));
#endif
  matrix_mul_hMx(p1,global_camera->persmatrix,p1);
  *x = p1[0]/p1[3];
  *y = p1[1]/p1[3];
  return 1;
}


int projectpoint_test_screen(float *pworld, float *x, float *y)
{
  float p1[4];

  p1[0] = pworld[0];
  p1[1] = pworld[1];
  p1[2] = pworld[2];
  p1[3] = 1.0;
  matrix_mul_hMx(p1,global_camera->posmatrix,p1);
  if (fabs(p1[3]) < EPS) return 0;
  if (p1[2]/p1[3] < EPS) return 0;
  matrix_mul_hMx(p1,global_camera->persmatrix,p1);
  *x = p1[0]/p1[3];
  *y = p1[1]/p1[3];
  return 1;
}


// Pixel accurate.
int zbuffer_visible(x,y,depth,xs,ys)
float x,y,depth;
int *xs,*ys;
{
  *xs = (int)(0.5 + (float)(image_get_xsize(global_renderer->image)-1)
	      *(global_camera->da+x)/global_camera->da/2.0);
  *ys = (int)(0.5 + (float)(image_get_ysize(global_renderer->image)-1)
	      *(1.0-(global_camera->db+y)/global_camera->db/2.0));
  if ((*xs < 0) || (*xs >= image_get_xsize(global_renderer->image)) ||
      (*ys < 0) || (*ys >= image_get_ysize(global_renderer->image))) return 2;
  if (global_renderer->zbuffer->zf[*ys][*xs] > depth) {
    global_renderer->zbuffer->zf[*ys][*xs] = depth;
    return 0;
  } else return 1;
}

// Pixel accurate?
int zbuffer_testvisibility(x,y,depth,xs,ys)
float x,y,depth;
int *xs,*ys;
{
  *xs = (int)(image_get_xsize(global_renderer->image)
	      *(global_camera->da+x)/global_camera->da/2.0);
  *ys = (int)(image_get_ysize(global_renderer->image)
	      *(1.0-(global_camera->db+y)/global_camera->db/2.0));
  if ((*xs < 0) || (*xs >= image_get_xsize(global_renderer->image)) ||
      (*ys < 0) || (*ys >= image_get_ysize(global_renderer->image))) return 2;
  if (global_renderer->zbuffer->zf[*ys][*xs] > depth) {
    return 0;
  } else return 1;
}

// Pixel accurate.
int zbuffer_test_screen(float x, float y, int *xs, int *ys)
{
  *xs = (int)(0.5 + (float)(image_get_xsize(global_renderer->image)-1)
	      *(global_camera->da+x)/global_camera->da/2.0);
  *ys = (int)(0.5 + (float)(image_get_ysize(global_renderer->image)-1)
	      *(1.0-(global_camera->db+y)/global_camera->db/2.0));
  if ((*xs < 0) || (*xs >= image_get_xsize(global_renderer->image)) ||
      (*ys < 0) || (*ys >= image_get_ysize(global_renderer->image))) return 0;
  return 1;
}

int rendertest_point(float *PinLocal, int *xx, int *yy)
{
  float PinWorld[3];
  float x,y,depth;
  int xs,ys;
  int vis;

  transform_p(PinLocal,global_tmatrix->pLW,PinWorld);
  if (projectpoint(PinWorld,&x,&y,&depth) == 0) return 0;
  if ((vis = zbuffer_testvisibility(x,y,depth,&xs,&ys)) != 0) return 0;
  *xx = xs;
  *yy = ys;
  return 1;
}

int render_point_test_zbuffer(float *PinLocal, int *xx, int *yy)
{
  float PinWorld[3];
  float x,y,depth;
  int xs,ys;
  int vis;

  transform_p(PinLocal,global_tmatrix->pLW,PinWorld);
  if (projectpoint(PinWorld,&x,&y,&depth) == 0) return 0;
  if ((vis = zbuffer_testvisibility(x,y,depth,&xs,&ys)) != 0) return 0;
  *xx = xs;
  *yy = ys;
  return 1;
}

int render_point_test_screen(float *PinLocal, int *xx, int *yy)
{
  float PinWorld[3];
  float x,y,depth;
  int xs,ys;
  int vis;
  int n;

  transform_p(PinLocal,global_tmatrix->pLW,PinWorld);
  if (projectpoint_test_screen(PinWorld,&x,&y) == 0) return -1;
  n = zbuffer_test_screen(x,y,&xs,&ys);
  *xx = xs;
  *yy = ys;
  if (n == 0) return 0;
  return 1;
}

void render_point(float *PinLocal, float *NinLocal)
{
  float PinWorld[3],NinWorld[3];
  float surf[4];
  float x,y,depth;
  int xs,ys;
  int vis;

  transform_p(PinLocal,global_tmatrix->pLW,PinWorld);
  transform_n(NinLocal,global_tmatrix->nLW,NinWorld);
  vector3_unite(NinLocal,NinLocal);
/*  (void)Displacement(&PinWorld,&NinWorld,PinLocal,NinLocal);
  TransformP(PinLocal,&(tmatrix.pLW),&PinWorld);
  TransformN(NinLocal,&(tmatrix.nLW),&NinWorld);
*/
  vector3_unite(NinWorld,NinWorld);

  if (projectpoint(PinWorld,&x,&y,&depth) == 0) return;
  if ((vis = zbuffer_visible(x,y,depth,&xs,&ys)) == 2) return;
  if (vis == 0) {
    (void)surface(PinWorld,NinWorld,surf);
    image_put_rgba_float(global_renderer->image,xs,ys,surf);
  }
}

int renderpoint_feedback(float *PinLocal, float *NinLocal, int *px, int *py)
{
  float PinWorld[3],NinWorld[3];
  float surf[4];
  float x,y,depth;
  int xs,ys;
  int vis;

  transform_p(PinLocal,global_tmatrix->pLW,PinWorld);
  transform_n(NinLocal,global_tmatrix->nLW,NinWorld);
  vector3_unite(NinLocal,NinLocal);
/*  (void)Displacement(&PinWorld,&NinWorld,PinLocal,NinLocal);
  TransformP(PinLocal,&(tmatrix.pLW),&PinWorld);
  TransformN(NinLocal,&(tmatrix.nLW),&NinWorld);
*/
  vector3_unite(NinWorld,NinWorld);
  if (projectpoint(PinWorld,&x,&y,&depth) == 0) return 0;
  if ((vis = zbuffer_visible(x,y,depth,&xs,&ys)) == 2) return 0;
  if (vis == 0) {
    (void)surface(PinWorld,NinWorld,surf);
    image_put_rgba_float(global_renderer->image,xs,ys,surf);
  }
  *px = xs;
  *py = ys;
  return 1;
}

#endif
