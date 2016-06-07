#ifndef EI_BUTTON_DRAW_H
#define EI_BUTTON_DRAW_H

#include "hw_interface.h"
#include "ei_types.h"
#include "ei_draw.h"
#include "ei_widget_frame.h" // pour alpha_handling
#include "ei_widget_button.h" // pour widget button

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef PI
#define PI  3.14159265359
#endif

enum dessin 
{
	HI,
	LO,
	ALL
};
//genere la liste de points chainés pour un arc
ei_linked_point_t* ei_arc_generate(ei_point_t center, 
				   int radius,
				   float start_agl, 
				   float end_agl);

//genere la liste de points pour un rectangle a bouts arrondis
ei_linked_point_t* ei_rounded_generate(ei_rect_t rectangle, int radius, enum dessin zone);

/* void light_draw(ei_surface_t surface, ei_button_t* button,  */
/* 		ei_linked_point_t* list, ei_rect_t* clipper); */

/* void dark_draw(ei_surface_t surface, */
/* 	       ei_button_t* button, */
/* 	       ei_linked_point_t* list, */
/* 	       ei_rect_t* clipper); */


/* void normal_draw(ei_surface_t surface, */
/* 		 ei_button_t* button, */
/* 		 ei_linked_point_t* list, */
/* 		 ei_rect_t* clipper); */

#endif
