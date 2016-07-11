
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include "gui.h"


#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL2_IMPLEMENTATION

#include "nuklear.h"
#include "nuklear_glfw_gl2.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])


struct nk_context *ctx;
struct nk_color background;

static strings_v country_names;

static void (*on_country)(int) = NULL;

void 
gui_init(
    GLFWwindow* win, 
    strings_v*  names,
    void (*callback)(int)) {

    country_names = *names;
    on_country = callback;

    /* GUI */
    ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
    {struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();
    }

    background = nk_rgb(28,48,62);
}


void 
gui_logic(int width, int height) {

    static int selected_country = -1; 
    
    nk_glfw3_new_frame();
    struct nk_panel layout;
    char title[256];
    sprintf(title, "%d x %d", width, height);

    if (nk_begin(ctx, &layout, title, nk_rect(10, 10, 300, 600),
        NK_WINDOW_BORDER      | 
        NK_WINDOW_MOVABLE     | 
        NK_WINDOW_SCALABLE    |
        NK_WINDOW_MINIMIZABLE | 
        NK_WINDOW_TITLE       |
        0)) {
        
        struct nk_panel tab;

        nk_layout_row_dynamic(ctx, 400, 1);
        if (nk_group_begin(ctx, &tab, "Group_Without_Border", 0)) {
            nk_layout_row_dynamic(ctx, 16, 2);
            for (int i = 0; i <country_names.n; ++i) {
                if (nk_button_label(ctx, country_names.a[i], NK_BUTTON_DEFAULT)) {
                    fprintf(stdout, "%d: %s\n", selected_country = i, country_names.a[i]);
                    if(on_country != NULL) on_country(i);
                }
            }
            nk_group_end(ctx);
        }

        if(selected_country >=0) {
            nk_layout_row_dynamic(ctx, 16, 1);
            nk_label(ctx, country_names.a[selected_country], NK_TEXT_CENTERED);
        } 
    }
    nk_end(ctx);
}

void 
gui_draw() {
    nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void 
gui_cleanup() {
    nk_glfw3_shutdown();
}