
#ifndef TRANSFORMS_H
#define TRANSFORMS_H

void transforms_init(); // global_transforms.
void transforms_free();
void transforms_new_name(char *);
void transforms_use_name(char *);
void transforms_pop_name();
void transforms_delete_name(char *);
void camera_fov(void *, float);
void camera_pers(void *);
void camera_pos(void *);
void transforms_setidentity();
void push();
void pop();
void tran(float, float, float);
void scal(float, float, float);
void rotx(float);
void roty(float);
void rotz(float);
void pers(float);
void pos(char *);

void tranrotscal(float *, float *, float);
void rot33(float *);

float deg2rad(float);
void vector3_crossprod(float *, float *, float *);
void vector3_unite(float *, float *);
float vector3_dotprod(float *, float *);

void vector3_copy(float *, float *);
void matrix4_copy(float *, float *);
float *matrix4_new();
void matrix4_genunit(float *);
void matrix_mul_hMx(float *, float *, float *);


void transform_p(float *, float *, float *);
void transform_n(float *, float *, float *);

float *transforms_get_pLW();
float *transforms_get_pWL();
float *transforms_get_nLW();
float *transforms_get_nWL();


#endif
