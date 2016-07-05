
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

#include "main.h"

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

void gui_init(GLFWwindow* win) {

    /* GUI */
    ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    {struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    // struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "DroidSans.ttf", 10, 0);
    // struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "Roboto-Regular.ttf", 14, 0);
    /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
    /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
    /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
    /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
    nk_glfw3_font_stash_end();
    // nk_style_set_font(ctx, &droid->handle);
    }

    background = nk_rgb(28,48,62);
}

void gui_logic() {
    
    // printf("logic\n");
        
    nk_glfw3_new_frame();

    /* GUI */
    {struct nk_panel layout;
    if (nk_begin(ctx, &layout, "Controls", nk_rect(10, 10, 230, 300),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
        
        nk_layout_row_dynamic(ctx, 25, 2);

            nk_property_float(ctx, "posx:" , -10.0, &posx , 10.0, 0.01, 0.01);
            nk_property_float(ctx, "posy:" , -10.0, &posy , 10.0, 0.01, 0.01);

        nk_layout_row_dynamic(ctx, 25, 1);

            nk_property_float(ctx, "scale:",   0.1, &scale, 10.0, 0.01, 0.01);
            
            nk_property_int(ctx, "level:", 0, &current_buffer, 9, 1, 1);
            if (nk_button_label(ctx, "reset", NK_BUTTON_DEFAULT)) {
                posx =  0.0;
                posy =  0.0;
                scale = 1.0;
                current_buffer = 5;
            }

        if (nk_tree_push(ctx, NK_TREE_TAB, "help", NK_MINIMIZED)) {

            nk_layout_row_dynamic(ctx, 14, 1);
                nk_label(ctx, "Keys:"             ,NK_TEXT_LEFT);
                nk_label(ctx, "cursor - translate",NK_TEXT_LEFT);
                nk_label(ctx, "0-9    - level"    ,NK_TEXT_LEFT);
                nk_label(ctx, "+/-    - zoom"     ,NK_TEXT_LEFT);
    
            nk_tree_pop(ctx);
        }

    }
    nk_end(ctx);}
}

void gui_draw() {
    nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void gui_cleanup() {
    nk_glfw3_shutdown();
}

