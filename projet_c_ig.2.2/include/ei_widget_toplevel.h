#ifndef EI_WIDGET_TOPLEVEL_H
#define EI_WIDGET_TOPLEVEL_H

#include "ei_widget.h"
#include "ei_draw.h"
#include "ei_button_draw.h"
#include "ei_types.h"
#include "hw_interface.h"
#include "ei_widget_frame.h"
#include "ei_widget_button.h"
#include "ei_widget_frame.h"
#include "ei_geometrymanager.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct ei_widgetclass_t *toplevel_class;

typedef struct ei_toplevel_t {
        ei_widget_t wtoplevel;

        //ici champs spécifiques a toplevel
        ei_color_t color;
        int border_width;
        char *title;
	ei_bool_t closable;
	ei_axis_set_t resizable;
	ei_size_t *min_size;
} ei_toplevel_t;

void* alloc_toplevel();
void release_toplevel(ei_widget_t* widget);
void draw_toplevel(struct ei_widget_t *widget, ei_surface_t surface, ei_surface_t pick_surface,
        ei_rect_t *clipper);
void setdefaults_toplevel(struct ei_widget_t* widget);
void geom_toplevel(struct ei_widget_t* widget, ei_rect_t rect);


#endif
