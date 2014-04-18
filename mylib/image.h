
#ifndef IMAGE_H
#define IMAGE_H

void *image_new(char *, int, int);
void *image_new_rgb(int, int);
void *image_new_g(int, int);
void *image_new_rgba(int, int);
void image_free(void *);
char *image_get_type(void *);
int image_get_xsize(void *);
int image_get_ysize(void *);
unsigned char *image_get_buffer(void *);
void *image_load_file(char *);
void *image_load_file_comp(char *);
void *image_load_file_jpeg(char *);
void *image_load_file_png(char *);
void image_save_file(void *, char *);
void image_save_stdout(void *);
void image_write2file_rgb_from_rgba(void *, char *);
void image_write2file_rgb_premultipliedalpha(void *, char *);
void image_write2file_rgb_premultipliedalpha_speedtree(void *, char *);
void image_write2file_alpha(void *, char *);

void image_put_rgba_float(void *, int, int, float *);
void image_get_rgb(void *, int, int, int *, int *, int *);
void image_put_rgb(void *, int, int, int, int, int);
void image_get_rgba(void *, int, int, int *, int *, int *, int *);
void image_put_rgba(void *, int, int, int, int, int, int);
void image_put_fz(void *, int, int, float);
float image_get_fz(void *, int, int);

void *image_convert_to_rgb(void *);
void image_copy_buffer(void *, unsigned char *);

void image_scale_zoom(void *, void *, float, float, float);
void image_scale_zoom_add(void *, void *, float, float, float);
void image_scale_zoom_add_area(void *, void *, float, float, float, int, int, int, int);
void image_scale_zoom_overwrite_area_viewer(void *, void *, float, float, float, float, int, int, int, int);
void image_scale_zoom_max_area(void *, void *, float, float, float, float, int, int, int, int);
void image_scale_zoom_max_area_bound(void *, void *, float, float, float, float, int, int, int, int, int, int, int, int);
void image_scale_zoom_max_area_bound_rgb2rgba(void *, void *, float, float, float, float, int, int, int, int, int, int, int, int);
void image_scale_zoom_area(void *, void *, float, float, float, int, int, int, int);

void image_copy_rectangle(void *,int,int,int,int,void *,int,int);
// void image_copy_rgb2rgba(void *,void *);
void image_copy_rgba2rgb(void *, void *);
void image_copy_rgba2rgba_alpha(void *, void *);
void image_copy_rgba2rgba_alpha_opacity(void *, void *, int);
void image_clean(void *);
void image_clean_rgba_transparent(void *);

void image_scale(void *, void *);

#endif

