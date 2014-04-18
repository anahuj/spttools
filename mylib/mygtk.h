

#ifndef MYGTK_H
#define MYGTK_H

// quick solution
// void *global_mygtk_active_callbacks_func[];
// void *global_mygtk_active_callbacks_funcdata[];

// XXX We use the GDK event numbers.

// XXX Old stuff:
// 0 configure
// 1 expose
// 2 motion notify
// 3 button press
// 4 button release
// 5 key press
// 6 key release
// 9 display timeout
// 10 realize

void mygtk_init();
void mygtk_main();

void *rgbda_new();
void rgbda_timeoutoff(void *);
void rgbda_timeouton(void *);
void rgbda_free(void *);
void rgbda_expose(void *, int, int, int, int);
void *rgbda_getrgbimage(void *);

void *openglda_new();
// void openglda_timeoutoff(void *);
// void openglda_timeouton(void *);
void openglda_free(void *);
// void openglda_expose(void *, int, int, int, int);
// void *openglda_getrgbimage(void *);
void *openglda_getgc(void *);
void openglda_expose(void *);

void callbacks_clear();
void callbacks_set_configure(int (*)(), void *);
void callbacks_set_expose(int (*)(), void *);
void callbacks_set_motionnotify(int (*)(), void *);
void callbacks_set_buttonpress(int (*)(), void *);
void callbacks_set_buttonrelease(int (*)(), void *);
void callbacks_set_keypress(int (*)(), void *);
void callbacks_set_keyrelease(int (*)(), void *);
void callbacks_set_displaytimeout(int (*)(), void *, int);
void callbacks_set_realize(int (*)(), void *);
void callbacks_set_leavenotify(int (*)(), void *);
void callbacks_set_enternotify(int (*)(), void *);

void topwindow(char *, int, int, void (*)(), void *);
void topwindow_end();
void vbox();
void vbox_end();
void hbox();
void hbox_end();
void table(int, int);
void table_end();
void tablecell(int, int, int);

void label(char *);

// void *lookactivebox();

// void *getcurrentwidget();
// void insert2container();

void addcallback_keypress(void (*)(), void *);
void addcallback_keyrelease(void (*)(), void *);

void *slider_new(int, int, int, float, float, float, void (*)(), void *);
void *slider_getslider(void *);
float slider_getvalue(void *);

#endif
