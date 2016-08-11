#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <cairo.h>
#include <cairo-xlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "colors.h"
#include "tttm.h"

int 
cairo_check_event(cairo_surface_t *sfc, int block) {   

   char keybuf[8];
   KeySym key;
   XEvent e;

   for (;;) {
      if (block || XPending(cairo_xlib_surface_get_display(sfc)))
         XNextEvent(cairo_xlib_surface_get_display(sfc), &e);
      else 
         return 0;

      switch (e.type) {
         case ButtonPress:
            return -e.xbutton.button;
         case KeyPress:
            XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
            return key;
         default:
            fprintf(stderr, "Dropping unhandled XEevent.type = %d.\n", e.type);
      }
   }
}


static void 
fullscreen(Display* dpy, Window win) {
  Atom atoms[2] = { XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False), None };
  XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_STATE", False),
                  XA_ATOM, 32, PropModeReplace, (unsigned char*) atoms, 1);
}


/*! Open an X11 window and create a cairo surface base on that window. If x and
 * y are set to 0 the function opens a full screen window and stores to window
 * dimensions to x and y.
 * @param x Pointer to width of window.
 * @param y Pointer to height of window.
 * @return Returns a pointer to a valid Xlib cairo surface. The function does
 * not return on error (exit(3)).
 */
cairo_surface_t* 
cairo_create_x11_surface(int *x, int *y) {

   Display *dsp;
   Drawable da;
   Screen *scr;
   int screen;
   cairo_surface_t *sfc;

   if ((dsp = XOpenDisplay(NULL)) == NULL)
      exit(1);
   screen = DefaultScreen(dsp);
   scr = DefaultScreenOfDisplay(dsp);
   if (!*x || !*y)
   {
      *x = WidthOfScreen(scr), *y = HeightOfScreen(scr);
      da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, *x, *y, 0, 0, 0);
      fullscreen (dsp, da);
   }
   else
      da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, *x, *y, 0, 0, 0);
   XSelectInput(dsp, da, ButtonPressMask | KeyPressMask);
   XMapWindow(dsp, da);

   sfc = cairo_xlib_surface_create(dsp, da, DefaultVisual(dsp, screen), *x, *y);
   cairo_xlib_surface_set_size(sfc, *x, *y);

   return sfc;
}

void 
cairo_close_x11_surface(cairo_surface_t *sfc) {
   Display *dsp = cairo_xlib_surface_get_display(sfc);

   cairo_surface_destroy(sfc);
   XCloseDisplay(dsp);
}


void 
draw_point(
   cairo_t* ctx, 
   point_t*  p, 
   char*    t) {
   cairo_move_to(ctx, p->x, p->y);
   cairo_show_text(ctx, t);
}


// void 
// draw_triangle(
//    cairo_t*    ctx, 
//    triangle_t* t, 
//    point_t*    f, 
//    point_t*    l) {
// 
//    // cairo_move_to(ctx, t->a->x, t->a->y);
//    // cairo_line_to(ctx, t->b->x, t->b->y);
//    // cairo_line_to(ctx, t->c->x, t->c->y);
//    // cairo_line_to(ctx, t->a->x, t->a->y);
//    // cairo_set_source_rgb(ctx, f->x, f->y, f->z);
//    // cairo_fill_preserve(ctx);
//    // cairo_set_line_width(ctx, 1);
//    // cairo_set_source_rgb(ctx, l->x, l->y, l->z);
//    // cairo_stroke(ctx);   
// 
//    // cairo_set_source_rgb(ctx, c->a->y, c->b->y, c->c->y);
//    // draw_point(ctx, t->a, "a");
//    // draw_point(ctx, t->b, "b");
//    // draw_point(ctx, t->c, "c");
// }


// split               z =             [0   1   2   3   4   5]  
// triangle side subs  n = 2^z+1     = [2   3   5   9  17  33] 
// points per triangle T = n*(n+1)/2 = [3   6  15  45 153 561]

// void
// generate_triangle_grid(
//    mesh_points_t*    points, 
//    mesh_triangles_t* triangles, 
//    triangle_t*       t, 
//    int               z ) {
// 
//    // size_t n = kv_size(*points);
//    // struct point_t p1 = (struct point_t) {t->a->x, t->a->y, t->a->z};
//    // struct point_t p2 = (struct point_t) {t->b->x, t->b->y, t->b->z};
//    // struct point_t p3 = (struct point_t) {t->c->x, t->c->y, t->c->z};
//    // kv_push(struct point_p, *points, p1);
//    // kv_push(struct point_p, *points, p2);
//    // kv_push(struct point_p, *points, p3);
// 
// }

void
mesh_draw(cairo_t* ctx, mesh_t* m) {
   
   for(uint32_t tid=0; tid<kv_size(m->triangles); tid++){
      
      triangle_t t = kv_A(m->triangles, tid);

      point_t p1 = kv_A(m->points, t.a);
      point_t p2 = kv_A(m->points, t.b);
      point_t p3 = kv_A(m->points, t.c);

      cairo_move_to(ctx, p1.x, p1.y);
      cairo_line_to(ctx, p2.x, p2.y);
      cairo_line_to(ctx, p3.x, p3.y);
      cairo_line_to(ctx, p1.x, p1.y);
      double r = rand()%100/100.0;
      double g = rand()%100/100.0;
      double b = rand()%100/100.0;
      cairo_set_source_rgb(ctx, r, g, b);
      cairo_fill_preserve(ctx);

      cairo_set_line_width(ctx, 1.0);
      cairo_set_source_rgb(ctx, 1.0, 1.0, 1.0);
      cairo_stroke(ctx);   
   }
}

mesh_t* 
mesh_create(int x, int y) {
    
   shape_t* s = shape_new();
   loop_t l;
   kv_init(l);
   uint32_t steps = 50;
   for(uint32_t i=0; i<steps; i++) {
      double dr = 150.0 * cos(2*3.1415926*i/steps*5);
      point_t p = (point_t) {
         x/2.0 + (200-dr) * cos(dr/250.0 + 2.0*3.1415926*i/steps),
         y/2.0 + (200-dr) * sin(dr/250.0 + 2.0*3.1415926*i/steps)};
      uint32_t pid = shape_add_point(s, p);
      kv_push(uint32_t, l, pid);
   }
   kv_push(loop_t, s->loops, l);

   return shape_triangulate(s);

}

int 
main(int argc, char** argv) {
   
   cairo_surface_t *sfc;
   cairo_t *ctx;
   
   int x, y;
   struct timespec ts = {0, 500000000};
   
   int running;

   x = y = 0;
   sfc = cairo_create_x11_surface(&x, &y);
   ctx = cairo_create(sfc);
   cairo_set_antialias(ctx, CAIRO_ANTIALIAS_NONE);

   mesh_t* m = mesh_create(x, y);

   for (running = 1; running;) {

      cairo_push_group(ctx);
      
         cairo_set_source_rgb(ctx, 0.1, 0.1, 0.1);
         cairo_paint(ctx);
         
         mesh_draw(ctx, m);

      cairo_pop_group_to_source(ctx);
      cairo_paint(ctx);

      cairo_surface_flush(sfc);

      int event=0;
      switch (event=cairo_check_event(sfc, 0)) {
         case 0xff53:   // right cursor
            break;

         case 0xff51:   // left cursor
            break;

         case 0xff1b:   // Esc
         case -1:       // left mouse button
            running = 0;
            break;
      }

      nanosleep(&ts, NULL);
   }

   mesh_free(m);

   cairo_destroy(ctx);
   cairo_close_x11_surface(sfc);

   return 0;
}

