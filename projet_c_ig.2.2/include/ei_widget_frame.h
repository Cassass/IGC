#ifndef EI_WIDGET_FRAME_H
#define EI_WIDGET_FRAME_H

#include "ei_widget.h"
#include "ei_draw.h"
#include "hw_interface.h"

#include <stdlib.h>

struct ei_widgetclass_t *frame_class;
//struct ei_widgetclass_t *toplevel_class;

typedef struct ei_frame_t {
	ei_widget_t wframe;

	//ici champs spécifiques a frame
	ei_color_t color;
	int border_width;
	ei_relief_t relief;
	char* text;
	ei_color_t text_color;
	ei_font_t text_font;
	ei_anchor_t text_anchor;
	ei_surface_t img;
	ei_rect_t* img_rect;
	ei_anchor_t img_anchor;
} ei_frame_t;

//fonctions pour la construction des fonctions ci-dessous
void insert_q_int(ei_linked_point_t** list, int x, int y);
void free_list(ei_linked_point_t** list);
ei_color_t ei_alpha_handling(ei_surface_t surface, ei_color_t top_color, 
			     ei_point_t pixel_coords, int offset);


//fonctions pour le widget
void* alloc_frame();
void release_frame(ei_widget_t* widget);
void draw_frame(struct ei_widget_t *widget, ei_surface_t surface, ei_surface_t pick_surface,
		ei_rect_t *clipper);
void setdefaults_frame(struct ei_widget_t* widget);
void geom_frame(struct ei_widget_t* widget, ei_rect_t rect);
/* void insert_q(ei_linked_point_t** list, int x, int y); */
#endif

