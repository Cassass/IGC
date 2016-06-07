#ifndef EI_WIDGET_BUTTON_H
#define EI_WIDGET_BUTTON_H

#include "ei_widget.h"
#include "ei_draw.h"
#include "ei_button_draw.h"
#include "ei_types.h"
#include "hw_interface.h"
#include <stdlib.h>
#include <stdio.h>

struct ei_widgetclass_t *button_class;

typedef struct ei_button_t {
        ei_widget_t wbutton;

        //ici champs spécifiques a button
        ei_color_t color;
        int border_width;
        int corner_radius;
        ei_relief_t relief;
        char *text;
        ei_font_t text_font;
        ei_color_t text_color;
        ei_anchor_t text_anchor;
        ei_surface_t img;
        ei_rect_t *img_rect;
        ei_anchor_t img_anchor;
        ei_callback_t callback;
        void* user_param;

} ei_button_t;

void* alloc_button();
void release_button(ei_widget_t* widget);
void draw_button(struct ei_widget_t *widget, ei_surface_t surface, ei_surface_t pick_surface,
        ei_rect_t *clipper);
void setdefaults_button(struct ei_widget_t* widget);
void geom_button(struct ei_widget_t* widget, ei_rect_t rect);

#endif
