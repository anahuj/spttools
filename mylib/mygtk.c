
// Verify if this comments is still valid.
// Delayed push should be rewritten.
// hbox_end() and vbox_end() should activate the delayed operations
// down to where hbox() and vbox() are located in the stack.

// What are these doing here?
// ty_rgbda
// ty_fader

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <GL/gl.h>
#include <GL/glu.h>
// #include "mygtk.h"
#include "area.h"
#include "rgbimage.h"

#define ControlMask (1<<2) // Xlib's definition -- has GTK its own?
#define ShiftMask   (1<<0) // Xlib's definition -- has GTK its own?

#define MYGTK_ABOXCOUNT 100
#define MYGTK_ABOXTYPE_HBOX 0
#define MYGTK_ABOXTYPE_VBOX 1
#define MYGTK_ABOXTYPE_TOPWINDOW 2
#define MYGTK_ABOXTYPE_TABLE 3

#define MYGTK_CALLBACKS 200
#define MYGTK_CALLBACK_DISPLAYTIMEOUT 101
#define MYGTK_CALLBACK_REALIZE 102


// We should have a proper system for handling the pointer-object
// relations.
typedef struct {
  GtkWidget *da; /* drawing area widget */
  //  GdkPixmap *pixmap; /* where the drawing or the image is held */
  // ty_rgbimage *rgbimage;
  void *rgbimage;
  // ty_displaylist *displaylist;
  gint displaytimeout;
  // int x,y; /* position on the fixed widget */
  // int xs,ys; /* size */
  int truexs,trueys; /* actual size */
  int state; /* == 0 means drawing; == 1 means moving */
  int (*callbacks_func[MYGTK_CALLBACKS])(); // additional callbacks
  void *callbacks_funcdata[MYGTK_CALLBACKS]; // additional callbacks
  int timeoutperiod;
  gint (*timeoutfunc)();
  void *timeoutfuncdata;
} ty_rgbda;

typedef struct {
  GtkWidget *da; /* drawing area widget */
  gint displaytimeout;
  // int x,y; /* position on the fixed widget */
  // int xs,ys; /* size */
  int truexs,trueys; /* actual size */
  int state; /* == 0 means drawing; == 1 means moving */
  int (*callbacks_func[MYGTK_CALLBACKS])(); // additional callbacks
  void *callbacks_funcdata[MYGTK_CALLBACKS]; // additional callbacks
  int timeoutperiod;
  gint (*timeoutfunc)();
  void *timeoutfuncdata;
  int idle_id;
  int animate;
  GdkGLConfig *glconfig;
} ty_openglda;

typedef struct {
#if 0
  void (**delayed_func)();
  void **delayed_data;
  void **delayed_data2;
  int delayed_next;
  int delayed_max;
#endif
  int (**active_callbacks_func)();
  void **active_callbacks_data;
  void *activeboxwidget[100];
  int activeboxtype[100]; // 0 = h/vbox, 1 = container, 2 = table cell
  int activeboxindex;
  int timeoutperiod; // in milliseconds
  void *last;
} ty_mygtk;


typedef struct {
  void (*func)();
  void *funcdata;
  GtkWidget *slider;
  GtkAdjustment *adjustment;
} ty_slider;


typedef struct {
  void *table;
  int xindex;
  int yindex;
  int type;
} ty_table;


ty_mygtk *global_mygtk;

void insertactivebox(void *);


// 0,0,rgbimage_getxsize(rgbimage),rgbimage_getysize(rgbimage),
// x,y,xsize,ysize,
// void rgbda_draw(GtkWidget *rgbda, ty_rgbimage *rgbimage,
void rgbda_draw(GtkWidget *rgbda, void *rgbimage,
		int x, int y, int xsize, int ysize)
{
  int xx,yy,xxsize,yysize;
  int k;

#if 0
  fprintf(stderr,"%i %i %i %i -- %i %i\n",x,y,xsize,ysize,
	  rgbimage_getxsize(rgbimage),
	  rgbimage_getysize(rgbimage));
#endif

  k = area_intersect_rel(x,y,xsize,ysize,0,0,
			 rgbimage_getxsize(rgbimage),
			 rgbimage_getysize(rgbimage),
			 &xx,&yy,&xxsize,&yysize);
  // fprintf(stderr,"rgbda_draw begin\n");
  // fprintf(stderr,"rgbda_draw: %i\n",(int)(rgbda->window));

  if (k != -1) {
    gdk_draw_rgb_image(rgbda->window,
		       rgbda->style->white_gc,
		       xx,yy,xxsize,yysize,
		       GDK_RGB_DITHER_NONE, // GDK_RGB_DITHER_NORMAL,
		       rgbimage_getrgbbufat(rgbimage,xx,yy),
		       rgbimage_getrowstride(rgbimage));
  }

  // Causes large flicker.
  // gtk_widget_queue_draw (rgbda);

  // fprintf(stderr,"rgbda_draw end\n");
}

void rgbda_expose(void *gp, int x, int y, int xsize, int ysize)
{
  ty_rgbda *p;

  p = (ty_rgbda *)gp;
  // fprintf(stderr,"rgbda_expose: da = %i\n",(int)(p->da));
  rgbda_draw(p->da, p->rgbimage, x, y, xsize, ysize);
  // fprintf(stderr,"rgbda_expose end\n");
}

/* Create a new backing pixmap of the appropriate size */
static gint
rgbda_configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer gp)
{
  ty_rgbda *p;
  // ty_rgbimage *old, *new;
  void *old, *new;

  // fprintf(stderr,"rgbda_confevent1\n");

  p = (ty_rgbda *)gp;
  old = p->rgbimage;
  rgbimage_free(old); // moved free() here as we uncommented the copy below

  // resize the rgbimage by deleting the old and creating the new
  new = rgbimage_new(widget->allocation.width,widget->allocation.height);
  // rgbimage_clean(new);  // no cleaning as it may just waste time
  // not needed as we have to expose the image to rgbimage elsewhere
  // rgbimage_copy(old,new);
  p->rgbimage = new;
#if 0
  fprintf(stderr,"new size: %i %i\n",
	  rgbimage_getxsize(new),rgbimage_getysize(new));
#endif

  // We set event->x and event->y to upper left corner of drawing area
  // in screen space for being able to configure window so that
  // the image remains in the same screen position.
  // XXXXX  Not implemented. How to get screen positions??


  // fprintf(stderr,"rgbda_confevent2\n");

  if (p->callbacks_func[GDK_CONFIGURE] != NULL)
    (p->callbacks_func[GDK_CONFIGURE])(p->rgbimage, event, p->callbacks_funcdata[GDK_CONFIGURE]);

#if 0
  // Not a good idea. The image is empty. The application
  // should fill the empty image first.
  rgbda_draw(p->da,p->rgbimage,
	     0, 0,
	     rgbimage_getxsize(new), rgbimage_getysize(new));
#endif

  // fprintf(stderr,"rgbda_confevent3\n");
  return TRUE;
}

// Redraw the screen from the backing pixmap
// Should we update only the area given in expose event? we do so
static gint
rgbda_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer gp)
{
  ty_rgbda *p;

  p = (ty_rgbda *)gp;

  // we have to call the external expose function first as there we
  // may update the main image first

  // fprintf(stderr,"rgbda_expose_event: da = %i\n",(int)(p->da));

  if (p->callbacks_func[GDK_EXPOSE] != NULL)
    (p->callbacks_func[GDK_EXPOSE])(p->rgbimage, event, p->callbacks_funcdata[GDK_EXPOSE]);

  // fprintf(stderr,"rgbda_expoevent1\n");
#if 0
  rgbda_draw(p->da,p->rgbimage,
	     event->area.x, event->area.y,
	     event->area.width, event->area.height);
#endif

  // fprintf(stderr,"rgbda_expoevent2\n");
  return FALSE;
}

static gint
rgbda_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer gp)
{
  ty_rgbda *p;

  p = (ty_rgbda *)gp;

  if (p->callbacks_func[GDK_BUTTON_PRESS] != NULL)
    (p->callbacks_func[GDK_BUTTON_PRESS])(p->rgbimage, event, p->callbacks_funcdata[GDK_BUTTON_PRESS]);

  return TRUE;
}

static gint
rgbda_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer gp)
{
  ty_rgbda *p;

  p = (ty_rgbda *)gp;

  if (p->callbacks_func[GDK_BUTTON_RELEASE] != NULL)
    (p->callbacks_func[GDK_BUTTON_RELEASE])(p->rgbimage, event, p->callbacks_funcdata[GDK_BUTTON_RELEASE]);

  return TRUE;
}

static gint
rgbda_key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer gp)
{
  ty_rgbda *p;

  p = (ty_rgbda *)gp;

  if (p->callbacks_func[GDK_KEY_PRESS] != NULL)
    (p->callbacks_func[GDK_KEY_PRESS])(p->rgbimage, event, p->callbacks_funcdata[GDK_KEY_PRESS]);

  return TRUE;
}

static gint
rgbda_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer gp)
{
  ty_rgbda *p;

  p = (ty_rgbda *)gp;

  if (p->callbacks_func[GDK_KEY_RELEASE] != NULL)
    (p->callbacks_func[GDK_KEY_RELEASE])(p->rgbimage, event, p->callbacks_funcdata[GDK_KEY_RELEASE]);

  return TRUE;
}

static gint
rgbda_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer gp)
{
  int x, y;
  GdkModifierType state;
  ty_rgbda *p;

  p = (ty_rgbda *)gp;

  if (event->is_hint) {
    gdk_window_get_pointer (event->window, &x, &y, &state);
    event->x = x;
    event->y = y;
    event->state = state;
  }

  if (p->callbacks_func[GDK_MOTION_NOTIFY] != NULL)
    (p->callbacks_func[GDK_MOTION_NOTIFY])(p->rgbimage, event, p->callbacks_funcdata[GDK_MOTION_NOTIFY]);

  return TRUE;
}

gint
rgbda_displaytimeout(gpointer gp)
{
  ty_rgbda *p;

  p = (ty_rgbda *)gp;
  
  // fprintf(stderr,"rgbda_displaytimeout1\n");
  if (p->callbacks_func[MYGTK_CALLBACK_DISPLAYTIMEOUT] != NULL)
    (p->callbacks_func[MYGTK_CALLBACK_DISPLAYTIMEOUT])(p->rgbimage, p->callbacks_funcdata[MYGTK_CALLBACK_DISPLAYTIMEOUT]);
  // fprintf(stderr,"rgbda_displaytimeout2\n");

  // return TRUE;
  return FALSE;
}

void *rgbda_new()
{
  ty_rgbda *p;
  int i;

  p = (ty_rgbda *)malloc(sizeof(ty_rgbda));

  // additional callbacks
  for (i = 0; i < MYGTK_CALLBACKS; i++) {
    p->callbacks_func[i] = global_mygtk->active_callbacks_func[i];
    p->callbacks_funcdata[i] = global_mygtk->active_callbacks_data[i];
  }

  // initial size only
  p->rgbimage = rgbimage_new(10,10);

  gtk_widget_push_visual(gdk_rgb_get_visual());
  gtk_widget_push_colormap(gdk_rgb_get_cmap());
  p->da = gtk_drawing_area_new();
  gtk_widget_pop_visual();
  gtk_widget_pop_colormap();

  gtk_drawing_area_size (GTK_DRAWING_AREA (p->da), 10, 10);

  insertactivebox(p->da);
  // gtk_box_pack_start (GTK_BOX (global_mygtk->active_box), p->da, TRUE, TRUE, 0);

  // fprintf(stderr,"rgbda_new: da = %i\n",(int)(p->da));

  gtk_widget_set_events (p->da, gtk_widget_get_events(p->da)
			 | GDK_EXPOSURE_MASK
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
			 // | GDK_KEY_PRESS_MASK
			 // | GDK_KEY_RELEASE_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK);

  gtk_widget_show (p->da);

  gtk_signal_connect (GTK_OBJECT (p->da), "expose_event",
		      (GtkSignalFunc) rgbda_expose_event, (gpointer)p);

  gtk_signal_connect (GTK_OBJECT(p->da),"configure_event",
		      (GtkSignalFunc) rgbda_configure_event, (gpointer)p);

  gtk_signal_connect (GTK_OBJECT (p->da), "motion_notify_event",
		      (GtkSignalFunc) rgbda_motion_notify_event, (gpointer)p);

  gtk_signal_connect (GTK_OBJECT (p->da), "button_press_event",
		      (GtkSignalFunc) rgbda_button_press_event, (gpointer)p);

  gtk_signal_connect (GTK_OBJECT (p->da), "button_release_event",
		      (GtkSignalFunc) rgbda_button_release_event, (gpointer)p);

#if 0
  gtk_signal_connect (GTK_OBJECT (p->da), "key_press_event",
		      (GtkSignalFunc) rgbda_key_press_event, (gpointer)p);

  gtk_signal_connect (GTK_OBJECT (p->da), "key_release_event",
		      (GtkSignalFunc) rgbda_key_release_event, (gpointer)p);
#endif

#if 1
  p->displaytimeout = gtk_timeout_add(global_mygtk->timeoutperiod,
				      (GtkFunction)rgbda_displaytimeout,
				      (gpointer)p);
  p->timeoutperiod = global_mygtk->timeoutperiod;
  p->timeoutfunc = rgbda_displaytimeout;
  p->timeoutfuncdata = p;

#endif


  return (void *)p;
}

void rgbda_timeoutoff(void *pp)
{
  ty_rgbda *p;

  p = (ty_rgbda *)pp;
  gtk_timeout_remove(p->displaytimeout);
}

void rgbda_timeouton(void *pp)
{
  ty_rgbda *p;

  p = (ty_rgbda *)pp;
  p->displaytimeout = gtk_timeout_add(p->timeoutperiod,
				      (GtkFunction)p->timeoutfunc,
				      (gpointer)p->timeoutfuncdata);
}

void *rgbda_getrgbimage(void *p)
{
  ty_rgbda *q;

  q = (ty_rgbda *)p;
  return q->rgbimage;
}


#if 0
// Not needed?

void openglda_draw(GtkWidget *openglda, void *rgbimage,
		   int x, int y, int xsize, int ysize)
{
  int xx,yy,xxsize,yysize;
  int k;

#if 0
  fprintf(stderr,"%i %i %i %i -- %i %i\n",x,y,xsize,ysize,
	  rgbimage_getxsize(rgbimage),
	  rgbimage_getysize(rgbimage));
#endif

  k = area_intersect_rel(x,y,xsize,ysize,0,0,
			 rgbimage_getxsize(rgbimage),
			 rgbimage_getysize(rgbimage),
			 &xx,&yy,&xxsize,&yysize);
  // fprintf(stderr,"rgbda_draw begin\n");
  // fprintf(stderr,"rgbda_draw: %i\n",(int)(rgbda->window));

  if (k != -1) {
    gdk_draw_rgb_image(rgbda->window,
		       rgbda->style->white_gc,
		       xx,yy,xxsize,yysize,
		       GDK_RGB_DITHER_NONE, // GDK_RGB_DITHER_NORMAL,
		       rgbimage_getrgbbufat(rgbimage,xx,yy),
		       rgbimage_getrowstride(rgbimage));
  }

  // Causes large flicker.
  // gtk_widget_queue_draw (rgbda);

  // fprintf(stderr,"rgbda_draw end\n");
}
#endif

#if 0
void openglda_expose(void *gp, int x, int y, int xsize, int ysize)
{
  ty_openglda *p;

  p = (ty_openglda *)gp;
  // fprintf(stderr,"rgbda_expose: da = %i\n",(int)(p->da));
  rgbda_draw(p->da, p->rgbimage, x, y, xsize, ysize);
  // fprintf(stderr,"rgbda_expose end\n");
}
#endif

static void
openglda_realize (GtkWidget *widget, gpointer gp)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
  ty_openglda *p;
  int r;

  p = (ty_openglda *)gp;

  // fprintf(stderr,"realize1\n");

  /*** OpenGL BEGIN ***/
  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return;

  r = 0;
  if (p->callbacks_func[MYGTK_CALLBACK_REALIZE] != NULL)
    r = (p->callbacks_func[MYGTK_CALLBACK_REALIZE])(widget, p->callbacks_funcdata[MYGTK_CALLBACK_REALIZE]);

  gdk_gl_drawable_gl_end (gldrawable);
  /*** OpenGL END ***/

  return;
}


/* Create a new backing pixmap of the appropriate size */
static gint
openglda_configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer gp)
{
  ty_openglda *p;
  GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
  GLfloat w = widget->allocation.width;
  GLfloat h = widget->allocation.height;
  GLfloat aspect;
  int r;

  p = (ty_openglda *)gp;

  // fprintf(stderr,"configure1\n");

  /*** OpenGL BEGIN ***/
  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return FALSE;

  glViewport (0, 0, w, h);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
#if 0
  if (w > h)
    {
      aspect = w / h;
      glFrustum (-aspect, aspect, -1.0, 1.0, 5.0, 60.0);
    }
  else
    {
      aspect = h / w;
      glFrustum (-1.0, 1.0, -aspect, aspect, 5.0, 60.0);
    }
#endif
  // Application may override the Frustum settings.
  if (p->callbacks_func[GDK_CONFIGURE] != NULL)
    r = (p->callbacks_func[GDK_CONFIGURE])(widget, event, p->callbacks_funcdata[GDK_CONFIGURE]);

  glMatrixMode (GL_MODELVIEW);

  gdk_gl_drawable_gl_end (gldrawable);
  /*** OpenGL END ***/

  return TRUE;
}

// Redraw the screen from the backing pixmap
// Should we update only the area given in expose event? we do so
static gint
openglda_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer gp)
{
  ty_openglda *p;
  GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
  int r;

  p = (ty_openglda *)gp;

  // fprintf(stderr,"expose1\n");

  /*** OpenGL BEGIN ***/
  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return FALSE;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity ();

  r = 0;
  // Is the widget as first argument ok? Do we need it?
  if (p->callbacks_func[GDK_EXPOSE] != NULL)
    r = (p->callbacks_func[GDK_EXPOSE])(widget, event, p->callbacks_funcdata[GDK_EXPOSE]);

  /* Swap buffers */
  if (gdk_gl_drawable_is_double_buffered (gldrawable))
    gdk_gl_drawable_swap_buffers (gldrawable);
  else
    glFlush ();

  gdk_gl_drawable_gl_end (gldrawable);
  /*** OpenGL END ***/

  // return FALSE;    Why FALSE is used in rgbda?
  return TRUE;
}

void openglda_expose(void *pp)
{
  ty_openglda *p;
  GdkEventExpose event;

  p = (ty_openglda *)pp;

  event.type = GDK_EXPOSE;
  event.window = p->da->window;
  event.send_event = FALSE;
  event.area.x = 0;
  event.area.y = 0;
  event.area.width = p->da->allocation.width;
  event.area.height = p->da->allocation.height;
  event.region = NULL;
  event.count = 0;

  (void)openglda_expose_event (p->da, &event, p);
}

static gint
openglda_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer gp)
{
  ty_openglda *p;
  int r;

  p = (ty_openglda *)gp;

  // fprintf(stderr,"press1\n");
  r = 0;
  if (p->callbacks_func[GDK_BUTTON_PRESS] != NULL)
    r = (p->callbacks_func[GDK_BUTTON_PRESS])(widget, event, p->callbacks_funcdata[GDK_BUTTON_PRESS]);

  if (r)
    gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);

  return TRUE; // Why shapes.c uses FALSE?
}

static gint
openglda_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer gp)
{
  ty_openglda *p;
  int r;

  p = (ty_openglda *)gp;

  // fprintf(stderr,"release1\n");
  r = 0;
  if (p->callbacks_func[GDK_BUTTON_RELEASE] != NULL)
    r = (p->callbacks_func[GDK_BUTTON_RELEASE])(widget, event, p->callbacks_funcdata[GDK_BUTTON_RELEASE]);

  if (r)
    gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);

  return TRUE;
}

static gint
openglda_key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer gp)
{
  ty_openglda *p;
  int r;

  p = (ty_openglda *)gp;

  r = 0;
  if (p->callbacks_func[GDK_KEY_PRESS] != NULL)
    r = (p->callbacks_func[GDK_KEY_PRESS])(widget, event, p->callbacks_funcdata[GDK_KEY_PRESS]);

  if (r)
    gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);

  return TRUE;
}

static gint
openglda_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer gp)
{
  ty_openglda *p;
  int r;

  p = (ty_openglda *)gp;

  r = 0;
  if (p->callbacks_func[GDK_KEY_RELEASE] != NULL)
    r = (p->callbacks_func[GDK_KEY_RELEASE])(widget, event, p->callbacks_funcdata[GDK_KEY_RELEASE]);

  if (r)
    gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);

  return TRUE;
}

static gint
openglda_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer gp)
{
  int x, y;
  GdkModifierType state;
  ty_openglda *p;
  int r;

  p = (ty_openglda *)gp;

  // fprintf(stderr,"motion1\n");

  if (event->is_hint) {
    gdk_window_get_pointer (event->window, &x, &y, &state);
    event->x = x;
    event->y = y;
    event->state = state;
  }

  r = 0;
  if (p->callbacks_func[GDK_MOTION_NOTIFY] != NULL)
    r = (p->callbacks_func[GDK_MOTION_NOTIFY])(widget, event, p->callbacks_funcdata[GDK_MOTION_NOTIFY]);

  if (r)
    gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);

  return TRUE;
}

static gint
openglda_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer gp)
{
  int x, y;
  GdkModifierType state;
  ty_openglda *p;
  int r;

  p = (ty_openglda *)gp;

  // fprintf(stderr,"enter1\n");

  r = 0;
  if (p->callbacks_func[GDK_ENTER_NOTIFY] != NULL)
    r = (p->callbacks_func[GDK_ENTER_NOTIFY])(widget, event, p->callbacks_funcdata[GDK_ENTER_NOTIFY]);

  if (r)
    gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);

  return TRUE;
}

static gint
openglda_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer gp)
{
  int x, y;
  GdkModifierType state;
  ty_openglda *p;
  int r;

  p = (ty_openglda *)gp;

  // fprintf(stderr,"leave1\n");

  r = 0;
  if (p->callbacks_func[GDK_LEAVE_NOTIFY] != NULL)
    r = (p->callbacks_func[GDK_LEAVE_NOTIFY])(widget, event, p->callbacks_funcdata[GDK_LEAVE_NOTIFY]);

  if (r)
    gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);

  return TRUE;
}

static int
openglda_idle (GtkWidget *widget)
{
  /* Invalidate the whole window. */
  gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);

  /* Update synchronously. */
  gdk_window_process_updates (widget->window, FALSE);

  return TRUE;
}

static void
openglda_idle_add (ty_openglda *p)
{
  if (p->idle_id == 0) {
    p->idle_id = g_idle_add_full (GDK_PRIORITY_REDRAW,
				  (GSourceFunc) openglda_idle,
				  p->da,
				  NULL);
  }
}

static void
openglda_idle_remove (ty_openglda *p)
{
  if (p->idle_id != 0) {
    g_source_remove (p->idle_id);
    p->idle_id = 0;
  }
}

static gint
openglda_map_event(GtkWidget *widget, GdkEvent *event, gpointer gp)
{
  ty_openglda *p;

  p = (ty_openglda *)gp;

  if (p->animate)
    openglda_idle_add(p);

  return TRUE;
}

static gint
openglda_unmap_event(GtkWidget *widget, GdkEvent *event, gpointer gp)
{
  ty_openglda *p;

  p = (ty_openglda *)gp;

  openglda_idle_remove(p);

  return TRUE;
}

static gint
openglda_visibility_notify_event(GtkWidget *widget, GdkEventVisibility *event, gpointer gp)
{
  ty_openglda *p;

  p = (ty_openglda *)gp;

  if (p->animate) {
    if (event->state == GDK_VISIBILITY_FULLY_OBSCURED)
      openglda_idle_remove(p);
    else
      openglda_idle_add(p);
  }

  return TRUE;
}

gint
openglda_displaytimeout(gpointer gp)
{
  ty_rgbda *p;

  p = (ty_rgbda *)gp;
  
  // fprintf(stderr,"rgbda_displaytimeout1\n");
  if (p->callbacks_func[MYGTK_CALLBACK_DISPLAYTIMEOUT] != NULL)
    (p->callbacks_func[MYGTK_CALLBACK_DISPLAYTIMEOUT])(p->rgbimage, p->callbacks_funcdata[MYGTK_CALLBACK_DISPLAYTIMEOUT]);
  // fprintf(stderr,"rgbda_displaytimeout2\n");

  // return TRUE;
  return FALSE;
}

void *openglda_new()
{
  ty_openglda *p;
  int i;

  p = (ty_openglda *)malloc(sizeof(ty_openglda));

  // additional callbacks
  for (i = 0; i < MYGTK_CALLBACKS; i++) {
    p->callbacks_func[i] = global_mygtk->active_callbacks_func[i];
    p->callbacks_funcdata[i] = global_mygtk->active_callbacks_data[i];
  }

  // initial size only
  // p->rgbimage = rgbimage_new(10,10);

  p->glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB    |
					   GDK_GL_MODE_DEPTH  |
					   GDK_GL_MODE_DOUBLE);
  if (p->glconfig == NULL) {
    fprintf(stderr,"Cannot find the double-buffered visual.\n");
    fprintf(stderr,"Trying single-buffered visual.\n");

    p->glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB   |
					     GDK_GL_MODE_DEPTH);
    if (p->glconfig == NULL) {
      fprintf(stderr,"No appropriate OpenGL-capable visual found.\n");
      exit(-1);
    }
  }

  // gtk_widget_push_visual(gdk_rgb_get_visual());
  // gtk_widget_push_colormap(gdk_rgb_get_cmap());
  p->da = gtk_drawing_area_new();
  // gtk_widget_pop_visual();
  // gtk_widget_pop_colormap();

  gtk_drawing_area_size (GTK_DRAWING_AREA (p->da), 10, 10);

  gtk_widget_set_gl_capability (p->da,
				p->glconfig,
				NULL,
				TRUE,
				GDK_GL_RGBA_TYPE);

  insertactivebox(p->da);
  // gtk_box_pack_start (GTK_BOX (global_mygtk->active_box), p->da, TRUE, TRUE, 0);

  // fprintf(stderr,"rgbda_new: da = %i\n",(int)(p->da));

#if 1
  // GTK 1.2 stuff

  // XXX should gtk_widget_add_events() be used instead?
  gtk_widget_set_events (p->da, gtk_widget_get_events(p->da)
			 | GDK_EXPOSURE_MASK
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_ENTER_NOTIFY_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
			 | GDK_KEY_PRESS_MASK
			 | GDK_KEY_RELEASE_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK
			 | GDK_VISIBILITY_NOTIFY_MASK);

  gtk_widget_show (p->da);

  gtk_signal_connect_after (GTK_OBJECT (p->da), "realize",
			    (GtkSignalFunc) openglda_realize, (gpointer)p);
  gtk_signal_connect (GTK_OBJECT (p->da), "expose_event",
		      (GtkSignalFunc) openglda_expose_event, (gpointer)p);

  gtk_signal_connect (GTK_OBJECT(p->da),"configure_event",
		      (GtkSignalFunc) openglda_configure_event, (gpointer)p);

  gtk_signal_connect (GTK_OBJECT (p->da), "motion_notify_event",
		      (GtkSignalFunc) openglda_motion_notify_event, (gpointer)p);
  gtk_signal_connect (GTK_OBJECT (p->da), "leave_notify_event",
		      (GtkSignalFunc) openglda_leave_notify_event, (gpointer)p);
  gtk_signal_connect (GTK_OBJECT (p->da), "enter_notify_event",
		      (GtkSignalFunc) openglda_enter_notify_event, (gpointer)p);

  gtk_signal_connect (GTK_OBJECT (p->da), "button_press_event",
		      (GtkSignalFunc) openglda_button_press_event, (gpointer)p);

  gtk_signal_connect (GTK_OBJECT (p->da), "button_release_event",
		      (GtkSignalFunc) openglda_button_release_event, (gpointer)p);

#if 1
  gtk_signal_connect (GTK_OBJECT (p->da), "key_press_event",
		      (GtkSignalFunc) openglda_key_press_event, (gpointer)p);

  gtk_signal_connect (GTK_OBJECT (p->da), "key_release_event",
		      (GtkSignalFunc) openglda_key_release_event, (gpointer)p);
#endif


  gtk_signal_connect (GTK_OBJECT (p->da), "map_event",
		    (GtkSignalFunc) openglda_map_event, (gpointer)p);
  gtk_signal_connect (GTK_OBJECT (p->da), "unmap_event",
		    (GtkSignalFunc) openglda_unmap_event,(gpointer)p );
  gtk_signal_connect (GTK_OBJECT (p->da), "visibility_notify_event",
		    (GtkSignalFunc) openglda_visibility_notify_event, (gpointer)p);
#endif

#if 0
  // GTK 2.0 stuff   XXXX does not work, do I really have gtk-2.0?? yes

  gtk_widget_add_events (p->da,
			 GDK_BUTTON_PRESS_MASK        |
			 GDK_BUTTON_RELEASE_MASK      |
			 GDK_EXPOSURE_MASK            |
			 GDK_POINTER_MOTION_MASK      |
			 GDK_POINTER_MOTION_HINT_MASK |
			 GDK_BUTTON1_MOTION_MASK      |
			 GDK_BUTTON2_MOTION_MASK      |
			 GDK_BUTTON_PRESS_MASK        |
                         GDK_BUTTON_RELEASE_MASK      |
			 GDK_VISIBILITY_NOTIFY_MASK);

  g_signal_connect_after (G_OBJECT (p->da), "realize",
                          G_CALLBACK (openglda_realize), (gpointer)p);
  g_signal_connect (G_OBJECT (p->da), "configure_event",
		    G_CALLBACK (openglda_configure_event), (gpointer)p);
  g_signal_connect (G_OBJECT (p->da), "expose_event",
		    G_CALLBACK (openglda_expose_event), (gpointer)p);

  g_signal_connect (G_OBJECT (p->da), "button_press_event",
		    G_CALLBACK (openglda_button_press_event), (gpointer)p);
  g_signal_connect (G_OBJECT (p->da), "button_release_event",
		    G_CALLBACK (openglda_button_release_event), (gpointer)p);
  g_signal_connect (G_OBJECT (p->da), "motion_notify_event",
		    G_CALLBACK (openglda_motion_notify_event), (gpointer)p);

  g_signal_connect (G_OBJECT (p->da), "map_event",
		    G_CALLBACK (openglda_map_event), (gpointer)p);
  g_signal_connect (G_OBJECT (p->da), "unmap_event",
		    G_CALLBACK (openglda_unmap_event),(gpointer)p );
  g_signal_connect (G_OBJECT (p->da), "visibility_notify_event",
		    G_CALLBACK (openglda_visibility_notify_event), (gpointer)p);
#endif

#if 0
  // Not used in openglda.

  p->displaytimeout = gtk_timeout_add(global_mygtk->timeoutperiod,
				      (GtkFunction)rgbda_displaytimeout,
				      (gpointer)p);
  p->timeoutperiod = global_mygtk->timeoutperiod;
  p->timeoutfunc = rgbda_displaytimeout;
  p->timeoutfuncdata = p;

#endif

  p->idle_id = 0;
  p->animate = 0;

  return (void *)p;
}

static void
openglda_toggle_animation(ty_openglda *p)
{
  p->animate = !p->animate;

  if (p->animate) {
    openglda_idle_add(p);
  } else {
    openglda_idle_remove(p);
    gdk_window_invalidate_rect (p->da->window, &p->da->allocation, FALSE);
  }
}

void *openglda_getgc(void *pp)
{
  ty_openglda *p;

  p = (ty_openglda *)pp;
  return (void *)(p->da->style->black_gc);
}

#if 0
void openglda_timeoutoff(void *pp)
{
  ty_rgbda *p;

  p = (ty_rgbda *)pp;
  gtk_timeout_remove(p->displaytimeout);
}

void openglda_timeouton(void *pp)
{
  ty_rgbda *p;

  p = (ty_rgbda *)pp;
  p->displaytimeout = gtk_timeout_add(p->timeoutperiod,
				      (GtkFunction)p->timeoutfunc,
				      (gpointer)p->timeoutfuncdata);
}

void *openglda_getrgbimage(void *p)
{
  ty_rgbda *q;

  q = (ty_rgbda *)p;
  return q->rgbimage;
}
#endif



void callbacks_clear()
{
  int i;

  for(i = 0; i < MYGTK_CALLBACKS; i++) global_mygtk->active_callbacks_func[i] = NULL;
  for(i = 0; i < MYGTK_CALLBACKS; i++) global_mygtk->active_callbacks_data[i] = NULL;
}

void callbacks_set_configure(int (*func)(), void *funcdata)
{
  global_mygtk->active_callbacks_func[GDK_CONFIGURE] = func;
  global_mygtk->active_callbacks_data[GDK_CONFIGURE] = funcdata;
}

void callbacks_set_expose(int (*func)(), void *funcdata)
{
  global_mygtk->active_callbacks_func[GDK_EXPOSE] = func;
  global_mygtk->active_callbacks_data[GDK_EXPOSE] = funcdata;
}

void callbacks_set_buttonpress(int (*func)(), void *funcdata)
{
  global_mygtk->active_callbacks_func[GDK_BUTTON_PRESS] = func;
  global_mygtk->active_callbacks_data[GDK_BUTTON_PRESS] = funcdata;
}

void callbacks_set_buttonrelease(int (*func)(), void *funcdata)
{
  global_mygtk->active_callbacks_func[GDK_BUTTON_RELEASE] = func;
  global_mygtk->active_callbacks_data[GDK_BUTTON_RELEASE] = funcdata;
}

void callbacks_set_keypress(int (*func)(), void *funcdata)
{
  global_mygtk->active_callbacks_func[GDK_KEY_PRESS] = func;
  global_mygtk->active_callbacks_data[GDK_KEY_PRESS] = funcdata;
}

void callbacks_set_keyrelease(int (*func)(), void *funcdata)
{
  global_mygtk->active_callbacks_func[GDK_KEY_RELEASE] = func;
  global_mygtk->active_callbacks_data[GDK_KEY_RELEASE] = funcdata;
}

void callbacks_set_motionnotify(int (*func)(), void *funcdata)
{
  global_mygtk->active_callbacks_func[GDK_MOTION_NOTIFY] = func;
  global_mygtk->active_callbacks_data[GDK_MOTION_NOTIFY] = funcdata;
}

void callbacks_set_enternotify(int (*func)(), void *funcdata)
{
  global_mygtk->active_callbacks_func[GDK_ENTER_NOTIFY] = func;
  global_mygtk->active_callbacks_data[GDK_ENTER_NOTIFY] = funcdata;
}

void callbacks_set_leavenotify(int (*func)(), void *funcdata)
{
  global_mygtk->active_callbacks_func[GDK_LEAVE_NOTIFY] = func;
  global_mygtk->active_callbacks_data[GDK_LEAVE_NOTIFY] = funcdata;
}

// XXX There is no GDK event for this. GTK2 uses glib's events;
// XXX as soon as they work, all event can be numbered with
// XXX glib's events, perhaps.
void callbacks_set_displaytimeout(int (*func)(), void *funcdata,
				  int timeoutperiod)
{
  global_mygtk->active_callbacks_func[MYGTK_CALLBACK_DISPLAYTIMEOUT] = func;
  global_mygtk->active_callbacks_data[MYGTK_CALLBACK_DISPLAYTIMEOUT] = funcdata;
  global_mygtk->timeoutperiod = timeoutperiod;
}

void callbacks_set_realize(int (*func)(), void *funcdata)
{
  global_mygtk->active_callbacks_func[MYGTK_CALLBACK_REALIZE] = func;
  global_mygtk->active_callbacks_data[MYGTK_CALLBACK_REALIZE] = funcdata;
}

#if 0
void delayed_push(void (*func)(), void *data, void *data2)
{
  ty_mygtk *p;

  p = global_mygtk;
  if (p->delayed_next <= p->delayed_max) {
    p->delayed_func[p->delayed_next] = func;
    p->delayed_data[p->delayed_next] = data;
    p->delayed_data2[p->delayed_next] = data2;
    p->delayed_next++;
  } else {
    fprintf(stderr,"delayed overflow\n");
    exit(-1);
  }
}
#endif

void pushactivebox_hbox(void *p)
{
  int n;

  global_mygtk->activeboxindex++;
  n = global_mygtk->activeboxindex;
  if (n >= MYGTK_ABOXCOUNT) {
    fprintf(stderr,"Activebox count too low\n");
    exit(-1);
  }
  global_mygtk->activeboxtype[n] = MYGTK_ABOXTYPE_HBOX;
  global_mygtk->activeboxwidget[n] = p;
}

void pushactivebox_vbox(void *p)
{
  int n;

  global_mygtk->activeboxindex++;
  n = global_mygtk->activeboxindex;
  if (n >= MYGTK_ABOXCOUNT) {
    fprintf(stderr,"Activebox count too low\n");
    exit(-1);
  }
  global_mygtk->activeboxtype[n] = MYGTK_ABOXTYPE_VBOX;
  global_mygtk->activeboxwidget[n] = p;
}

void pushactivebox_topwindow(void *p)
{
  int n;

  global_mygtk->activeboxindex++;
  n = global_mygtk->activeboxindex;
  if (n >= MYGTK_ABOXCOUNT) {
    fprintf(stderr,"Activebox count too low\n");
    exit(-1);
  }
  global_mygtk->activeboxtype[n] = MYGTK_ABOXTYPE_TOPWINDOW;
  global_mygtk->activeboxwidget[n] = p;
}

void pushactivebox_table(void *p)
{
  int n;

  global_mygtk->activeboxindex++;
  n = global_mygtk->activeboxindex;
  if (n >= MYGTK_ABOXCOUNT) {
    fprintf(stderr,"Activebox count too low\n");
    exit(-1);
  }
  global_mygtk->activeboxtype[n] = MYGTK_ABOXTYPE_TABLE;
  global_mygtk->activeboxwidget[n] = p;
}

void table_attach(ty_table *p, void *q)
{
  int x,y;

  x = p->xindex;
  y = p->yindex;
  if (p->type == 0) {
  gtk_table_attach (GTK_TABLE (p->table), q, x, x+1, y, y+1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (GTK_EXPAND), 10, 0);
  } else {
  gtk_table_attach (GTK_TABLE (p->table), q, x, x+1, y, y+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 10, 0);
  }
}

void insertactivebox(void *p)
{
  int n, type;
  void *q;
  // int x,y;

  n = global_mygtk->activeboxindex;
  type = global_mygtk->activeboxtype[n];
  q = global_mygtk->activeboxwidget[n];
  // x = global_mygtk->activeboxintpar1[n];
  // y = global_mygtk->activeboxintpar2[n];
  switch (type) {
  case MYGTK_ABOXTYPE_HBOX:
  case MYGTK_ABOXTYPE_VBOX:
    // fprintf(stderr,"boxpack %i\n",(int)q);
    gtk_box_pack_start(q, p, TRUE, TRUE, 0); // for DA
    // gtk_box_pack_start(q, p, FALSE, FALSE, 0); // why??  slider!
    // Insert command depends also on the inserted object. 
    break;
  case MYGTK_ABOXTYPE_TOPWINDOW:
    gtk_container_add(q, p);
    break;
  case MYGTK_ABOXTYPE_TABLE:
    table_attach(q,p);
    break;
  default:
    break;
  }
}

void *popactivebox()
{
  int n;
  void *p;

  n = global_mygtk->activeboxindex;
  if (n == -1) return NULL;
  p = global_mygtk->activeboxwidget[n];
  global_mygtk->activeboxindex--;
  return p;
}

void *lookactivebox()
{
  int n;

  n = global_mygtk->activeboxindex;
  if (n == -1) return NULL;
  return global_mygtk->activeboxwidget[n];
}

void setlast(void *p)
{
  global_mygtk->last = p;
}

void *getlast()
{
  return global_mygtk->last;
}

void topwindow(char *title, int xsize, int ysize,
	       void (*destroycallback)(),
	       void *destroydata)
{
  GtkWidget *window;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  // Doesn't work: gtk_widget_set_usize(window,600,400);
  // Does work: gtk_window_set_default_size(window,600,400);
  gtk_window_set_default_size(GTK_WINDOW(window),xsize,ysize);

  gtk_window_set_title (GTK_WINDOW(window), title);
  gtk_container_set_reallocate_redraws (GTK_CONTAINER (window), TRUE);

#if 0
  gtk_signal_connect(GTK_OBJECT(window), "destroy",
                     GTK_SIGNAL_FUNC(destroycallback),
		     (gpointer)destroydata);
#else
  // GTK 2.0
  g_signal_connect(G_OBJECT(window), "delete_event",
		   G_CALLBACK(destroycallback),
		   (gpointer)destroydata);
#endif

  pushactivebox_topwindow(window);
  // delayed_push(gtk_widget_show, window, NULL);
}

void topwindow_end()
{
  void *p;

  p = popactivebox();
  gtk_widget_show(p);
  setlast(p);
}

void hbox()
{
  void *p;

  p = gtk_hbox_new(FALSE, 0);
  insertactivebox(p);
  pushactivebox_hbox(p);
  // push(gtk_widget_show, p, NULL);
}

void hbox_end()
{
  void *p;

  p = popactivebox();
  gtk_widget_show(p);
  setlast(p);
}

void vbox()
{
  void *p;

  p = gtk_vbox_new(FALSE, 0);
  insertactivebox(p);
  pushactivebox_vbox(p);
  // push(gtk_widget_show, p, NULL);
  // fprintf(stderr,"vbox %i\n",(int)p);
}

void vbox_end()
{
  void *p;

  p = popactivebox();
  gtk_widget_show(p);
  setlast(p);
}

ty_table *table_new(int x, int y)
{
  ty_table *p;

  p = (ty_table *)malloc(sizeof(ty_table));
  p->table = gtk_table_new(y,x,FALSE);
  p->xindex = 0;
  p->yindex = 0;
  p->type = 0;
  return p;
}

void table_setcell(ty_table *p, int x, int y, int type)
{
  p->xindex = x;
  p->yindex = y;
  p->type = type;
}


void table(int x, int y)
{
  ty_table *p;

  p = table_new(x,y);
  gtk_widget_show(p->table);
  insertactivebox(p->table);
  table_setcell(p,0,0,0);
  pushactivebox_table(p);
}

void tablecell(int x, int y, int type)
{
  void *p;

  p = lookactivebox();
  table_setcell(p,x,y,type);
}

void table_end()
{
  ty_table *p;

  p = popactivebox();
  setlast(p->table);
}

#if 0
void insert2container()
{
  delayed_push(gtk_container_add, global_mygtk->active_box2,
	       global_mygtk->active_box);
}

void containeradd(void *box, void *item)
{
  delayed_push(gtk_container_add, box, item);
}
#endif

// only after topwindow??
void addcallback_keypress(void (*func)(), void *funcdata)
{
  void *p;

  p = lookactivebox();
  gtk_signal_connect (GTK_OBJECT (p),
		      "key_press_event",
                      (GtkSignalFunc) func, (gpointer)funcdata);
  gtk_widget_set_events (p,
			 gtk_widget_get_events(p)
                         | GDK_KEY_PRESS_MASK);
}

void addcallback_keyrelease(void (*func)(), void *funcdata)
{
  void *p;

  p = lookactivebox();
  gtk_signal_connect (GTK_OBJECT (p),
		      "key_release_event",
                      (GtkSignalFunc) func, (gpointer)funcdata);
  gtk_widget_set_events (p,
			 gtk_widget_get_events(p)
                         | GDK_KEY_RELEASE_MASK);
}

void mygtk_init(int *ac, char ***av)
{
  ty_mygtk *p;
  int i;

  p = (void *)malloc(sizeof(ty_mygtk));
  p->activeboxindex = -1;

#if 0
  // delayed operations; general stack could be used instead of this
  p->delayed_next = 0;
  p->delayed_max = 100-1;
  p->delayed_func = (void (**)())malloc(100*sizeof(void (*)()));
  p->delayed_data = (void **)malloc(100*sizeof(void *));
  p->delayed_data2 = (void **)malloc(100*sizeof(void *));
  for(i = 0; i < 100; i++) p->delayed_func[i] = NULL;
  for(i = 0; i < 100; i++) p->delayed_data[i] = NULL;
  for(i = 0; i < 100; i++) p->delayed_data2[i] = NULL;
#endif

  // callback system
  p->active_callbacks_func = (int (**)())malloc(MYGTK_CALLBACKS*sizeof(void (*)()));
  p->active_callbacks_data = (void **)malloc(MYGTK_CALLBACKS*sizeof(void *));
  for(i = 0; i < MYGTK_CALLBACKS; i++) p->active_callbacks_func[i] = NULL;
  for(i = 0; i < MYGTK_CALLBACKS; i++) p->active_callbacks_data[i] = NULL;

  global_mygtk = p;

  gtk_init(ac, av);
  gdk_rgb_init();
  gtk_gl_init (ac, av);
}

void mygtk_main()
{
  ty_mygtk *p;
  int i;

  p = global_mygtk;
#if 0
  // perform delayed operations
  for (i = p->delayed_next - 1; i >= 0; i--) {
    if (p->delayed_data2[i] != NULL) {
      (p->delayed_func[i])(p->delayed_data[i],p->delayed_data2[i]);
    } else if (p->delayed_data[i] != NULL) {
      (p->delayed_func[i])(p->delayed_data[i]);
    } else {
      (p->delayed_func[i])(p->delayed_data[i]);
    }
  }
#endif

  gtk_main();
}


void label(char *s)
{
  void *p;

  p = gtk_label_new(s);
  gtk_widget_show(p);
  insertactivebox(p);
  setlast(p);
}


void *slider_new(int len, int orie, int drawval,
		 float minval, float maxval, float initval,
		 void (*func)(), void *funcdata)
{
  ty_slider *p;

  p = (ty_slider *)malloc(sizeof(ty_slider));
  p->func = func;
  p->funcdata = funcdata;
  p->adjustment = GTK_ADJUSTMENT(gtk_adjustment_new(initval, minval, maxval,
						    0.0, 0.0, 0.0));
  if (orie == 0) p->slider = gtk_vscale_new(p->adjustment);
  else p->slider = gtk_hscale_new(p->adjustment);
  gtk_signal_connect(GTK_OBJECT(p->adjustment), "value_changed",
		     GTK_SIGNAL_FUNC(func), (gpointer)funcdata);
  if (drawval == 0) gtk_scale_set_draw_value((GtkScale *)p->slider,FALSE);
  else gtk_scale_set_draw_value((GtkScale *)p->slider,TRUE);
  gtk_scale_set_digits((GtkScale *)p->slider,3);
#if 0
  if (orie == 0) {
    gtk_widget_set_usize(p->slider,19,len);
  } else {
    gtk_widget_set_usize(p->slider,len,19);
  }
#endif
  
  // q = lookactivebox();
  insertactivebox(p->slider);
  // gtk_box_pack_start(q, p->slider, FALSE, FALSE, 0);
  gtk_widget_show(p->slider);
  setlast(p->slider);
  return (void *)p;
}

void *slider_getslider(void *gp)
{
  ty_slider *p;

  p = (ty_slider *)gp;
  return (void *)p->slider;
}

float slider_getvalue(void *gp)
{
  ty_slider *p;

  p = (ty_slider *)gp;
  // this may break, but GTK has no gtk_adjustment_get_value()  :-(
  return p->adjustment->value;
}

