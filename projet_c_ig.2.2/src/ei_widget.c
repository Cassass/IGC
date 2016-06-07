#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_widget_frame.h"
#include "ei_event.h"
#include "binding.h"
#include "hw_interface.h"
#include "ei_application.h"
#include "ei_geometrymanager.h"
#include "ei_widget_button.h"
#include "ei_widget_toplevel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t couleur = 100;

ei_widget_t* ei_widget_create(ei_widgetclass_name_t class_name, ei_widget_t* parent)
{
        /* printf("début widget_create\n"); */

        struct ei_widget_t *widget_cree = NULL;
        // cherche la classe du widget qui correspond à son nom:
        ei_widgetclass_t* class = ei_widgetclass_from_name(class_name);
        
        if ( strncmp(ei_widgetclass_stringname(class_name), "frame", 5) == 0 ) {
                struct ei_frame_t *frame = class->allocfunc ();
                widget_cree = &frame->wframe;
        } else if ( strncmp(ei_widgetclass_stringname(class_name), "button", 6) == 0) {
                struct ei_button_t *button = class->allocfunc ();
                widget_cree = &button->wbutton;
        } else if ( strncmp(ei_widgetclass_stringname(class_name), "toplevel", 8) == 0 ) {
                struct ei_toplevel_t *toplevel = class->allocfunc ();
                widget_cree = &toplevel->wtoplevel;
        }
       
        // on configure les attributs du widget
        widget_cree->wclass = class;
        widget_cree->pick_color = calloc(1, sizeof (ei_color_t));
        if (widget_cree->pick_color == NULL) {
                printf("widget_cree->wclass = NULL\n");
                exit(EXIT_FAILURE);
        }
        
        widget_cree->geom_params = calloc(1, sizeof(struct ei_geometry_param_t));
        if (widget_cree->geom_params == NULL) {
                printf("widget_cree->geom_params = NULL\n");
                exit(EXIT_FAILURE);
        }
	
        widget_cree->content_rect = calloc(1, sizeof (ei_rect_t));
        if (widget_cree->content_rect == NULL) {
                printf("widget_cree->content_rect = NULL\n");
                exit(EXIT_FAILURE);
        } 
       
        strcpy(widget_cree->wclass->name, class_name);
        widget_cree->pick_id = couleur;
        couleur += 100;         

        widget_cree->parent = parent;

        if (parent != NULL) {
                //printf("parent != null\n");
                struct ei_widget_t *cour = widget_cree->parent;
                if (cour->children_head == NULL) {
                        //printf("cas ou pas de fils\n");
                        cour->children_head = widget_cree;
                } else {
			// printf("cas ou plusieurs fils\n");
                        cour = cour->children_head;
                        while (cour->next_sibling != NULL) {
                                cour = cour->next_sibling;
                        }
                        cour->next_sibling = widget_cree;
                }
                widget_cree->parent->children_tail = widget_cree;
        }
/*
  else {
  //printf("parent = null\n");
  }
*/
        widget_cree->children_head = NULL;
        widget_cree->children_tail = NULL;
        widget_cree->next_sibling = NULL;
        
        printf("%u\n", widget_cree->pick_id);
        
        /* printf("sortie de widget_create\n"); */
        return widget_cree;

}

void ei_widget_destroy(ei_widget_t* widget)
{
        // il faut l'enlever de l'écran
}

///////////////////////////////////////////////////////////////////////////////



ei_widget_t * chercher_pick_color(uint8_t *pointeur, ei_widget_t * last)
{
	/* printf ("debut chercher_pick_color\n"); */
        ei_widget_t *racine = ei_app_root_widget();
        ei_widget_t *cour = ei_app_root_widget();
        ei_color_t *color_click = calloc (1, sizeof(ei_color_t));
        int ir, ig, ib, ia;
        hw_surface_get_channel_indices(surface_offscreen, &ir, &ig, &ib, &ia);
        if (color_click == NULL) {
                printf("color_click = null\n");
                exit(EXIT_FAILURE);
        }
/*
  printf ("ia = %u ib=%u ig=%u ir=%u\n", ia, ib, ig, ir);
*/
        color_click->alpha = *(pointeur + (ia));
        color_click->blue = *(pointeur + (ib));
        color_click->green = *(pointeur + (ig));
        color_click->red = *(pointeur + (ir));
        
        uint32_t pick_id_click = ei_map_rgba (surface_offscreen, color_click);
/*
  printf("pick_id_click = %u\n", pick_id_click);
*/
        while (pick_id_click != ei_map_rgba(ei_app_root_surface(),cour->pick_color)) {
/*
  printf("%x ---------- %x \n", pick_id_click, ei_map_rgba(ei_app_root_surface(), cour->pick_color));
*/
                if (cour == racine) {
                        if (racine ->children_head != NULL) {
                                cour = racine->children_head;
                        } else {
/*
  printf("racine ->children_head == null \n");
*/
                                exit(EXIT_FAILURE);
                        }
                } else {
                        if (cour->children_head == NULL) {
                                if (cour == last) {
                                        exit (EXIT_FAILURE);
                                }
                                while (cour == cour->parent->children_tail) {
                                        cour = cour->parent;
                                }
                                cour = cour->next_sibling;
                        } else {
                                cour = cour->children_head;
                        }
                }
        }
	if (cour == NULL) {
		/* printf("cour = null en sortie de fin chercher_pick_color\n"); */
	}
	/* printf("fin chercher_pick_color\n"); */
        return cour;
}


ei_widget_t* ei_widget_pick(ei_point_t* where)
{



        struct ei_widget_t *wid_racine = ei_app_root_widget ();
/*
  printf("pick_id racine=%u\n", wid_racine->pick_id);
*/
        //il faut determiner pick_id en x,y 
        //surement grace a l'offscreen ??

        
        uint8_t* pointeur_position = hw_surface_get_buffer(surface_offscreen);
        pointeur_position += 4*(where->y*wid_racine->screen_location.size.width + where->x);
/*
        
  printf("%u %u %u %u\n", *pointeur_position ,  *(pointeur_position +1),*(pointeur_position +2),*(pointeur_position +3));
*/
        struct ei_widget_t *cour = wid_racine;
        struct ei_widget_t *last = wid_racine;
        //on met dans last le dernier enfant (c'est a dire celui que l'on retrouve en suivant la liste des children )
        while (last->children_tail != NULL) {
                last = last->children_tail;
        }

	

              
        return chercher_pick_color(pointeur_position,last);;

      
}
////////////////////////////////////////////////////////////////////////////////// 


void ei_frame_configure(ei_widget_t* widget,
                        ei_size_t* requested_size,
                        const ei_color_t* color,
                        int* border_width,
                        ei_relief_t* relief,
                        char** text,
                        ei_font_t* text_font,
                        ei_color_t* text_color,
                        ei_anchor_t* text_anchor,
                        ei_surface_t* img,
                        ei_rect_t** img_rect,
                        ei_anchor_t* img_anchor)
{
        struct ei_frame_t *frame = (struct ei_frame_t*) widget;
        // si ce n'est pas la racine
        if (widget != ei_app_root_widget()) {
		if (text_font == NULL) {
			frame->text_font = ei_default_font;
		} else {
			frame->text_font = *text_font;
		}
	
		if (requested_size == NULL) {
                        if (text == NULL && img == NULL) {
                                widget->requested_size.height = 0;
                                widget->requested_size.width = 0;
                        } else if (text == NULL && img != NULL) {
                                widget->requested_size.height = 0;
                                widget->requested_size.width = 0;
                        } else if (text != NULL && img == NULL) {
                                hw_text_compute_size(*text, frame->text_font, &(widget->requested_size.width), &(widget->requested_size.height));
                        } else {
                                widget->requested_size.height = 0;
                                widget->requested_size.width = 0;
                        }
                } else {
                        widget->requested_size = *requested_size;
                }
        }
        
	if (color == NULL) {
		frame->color = ei_default_background_color;
	} else {
		frame->color.red = color->red;
		frame->color.green = color->green;
		frame->color.blue = color->blue;
		frame->color.alpha = color->alpha;
	}
	if (border_width == NULL) {
		frame->border_width = 0;
	} else {
		frame->border_width = *border_width;
	}
	if (relief == NULL) {
		frame->relief = ei_relief_none;
	} else {
		frame->relief = *relief;
	}
	if (text == NULL) {
		frame->text = NULL;
	} else {
		frame->text = *text;
	}
	if (text_color == NULL) {
		frame->text_color = ei_font_default_color;
	} else {
		frame->text_color = *text_color;
	}
	if (text_anchor == NULL) {
		frame->text_anchor = ei_anc_center;
	} else {
		frame->text_anchor = *text_anchor;
	}
	if (img == NULL) {
		frame->img = NULL;
	} else {
		frame->img = *img;
	}
	if (img_rect == NULL) {
		frame->img_rect = NULL;
	} else {
		frame->img_rect = *img_rect;
	}
	if (img_anchor == NULL) {
		frame->img_anchor = ei_anc_center;
	} else {
		frame->img_anchor = *img_anchor;
	}
        
        int ir, ig, ib, ia;
        hw_surface_get_channel_indices(surface_offscreen, &ir, &ig, &ib, &ia);
        
  
        uint32_t tmp = 0;
        tmp = widget->pick_id << (3 - ir)*8;
        tmp = tmp >> 24;
        widget->pick_color->red = (unsigned char) tmp;
        tmp = widget->pick_id << (3-ig)*8;
        tmp = tmp >> 24;
        widget->pick_color->green = (unsigned char) tmp;
        tmp = widget->pick_id << (3-ib)*8;
        tmp = tmp >> 24;
        widget->pick_color->blue = (unsigned char) tmp;
        tmp = widget->pick_id << (3-ia)*8;
        //tmp = tmp >> 24;
        widget->pick_color->alpha = 255;
}

void ei_button_configure(ei_widget_t* widget,
                         ei_size_t* requested_size,
                         const ei_color_t* color,
                         int* border_width,
                         int* corner_radius,
                         ei_relief_t* relief,
                         char** text,
                         ei_font_t* text_font,
                         ei_color_t* text_color,
                         ei_anchor_t* text_anchor,
                         ei_surface_t* img,
                         ei_rect_t** img_rect,
                         ei_anchor_t* img_anchor,
                         ei_callback_t* callback,
                         void** user_param)
{
	struct ei_button_t *button = (struct ei_button_t*) widget; 
	
        // A AMELIORER quand on aura avancer
	if (text_font == NULL) {
		button->text_font = ei_default_font;
	} else {
		button->text_font = *text_font;
	}       
       
	if (requested_size == NULL) {
                if (text == NULL && img == NULL) {
                        widget->requested_size.height = 0;
                        widget->requested_size.width = 0;
                } else if (text == NULL && img != NULL) {
                        widget->requested_size.height = 0;
                        widget->requested_size.width = 0;
                } else if (text != NULL && img == NULL) {
			// text forcement != NULL : pas de pb sur le dereferencement
                        hw_text_compute_size(*text, button->text_font, &widget->requested_size.width, &widget->requested_size.height);
                } else {
                        widget->requested_size.height = 0;
                        widget->requested_size.width = 0;
                }
        } else {
                widget->requested_size = *requested_size;
        }


        if (color == NULL) {
                button->color = ei_default_background_color;
        } else {
                button->color.red = color->red;
                button->color.green = color->green;
                button->color.blue = color->blue;
                button->color.alpha = color->alpha;
        }
       
        if (border_width == NULL) {
		button->border_width = k_default_button_border_width;
	} else {
		button->border_width = *border_width;
	}
        
	if (corner_radius == NULL) {
		button->corner_radius = k_default_button_corner_radius;
	} else {
		button->corner_radius = *corner_radius;
	}
       
	if (relief == NULL) {
		button->relief = ei_relief_raised;
	} else {
		button->relief = *relief;
	}
       
	if (text == NULL) {
		button->text = NULL;
	} else {
		button->text = *text;
	}
       
	if (text_color == NULL) {
		button->text_color = ei_font_default_color;
	} else {
		button->text_color = *text_color;
	}
       
	if (text_anchor == NULL) {
		button->text_anchor = ei_anc_center;
	} else {
		button->text_anchor = *text_anchor;
	}
       
	if (img == NULL) {
		button->img = NULL;
	} else {
		button->img = *img;
	}
       
	if (img_rect == NULL) {
		button->img_rect = NULL;
	} else {
		button->img_rect = *img_rect;
	}
       
	if (img_anchor == NULL) {
		button->img_anchor = ei_anc_center;
	} else {
		button->img_anchor = *img_anchor;
	}
       
        if (callback == NULL) {
                button->callback = NULL;
        } else {
                button->callback = *callback;
        }               
        
        if (user_param == NULL) {
                button->user_param = NULL;
        } else {
                button->user_param = *user_param;
        }
        
        int ir, ig, ib, ia;
        hw_surface_get_channel_indices(surface_offscreen, &ir, &ig, &ib, &ia);
        
  
        uint32_t tmp = 0;
        tmp = widget->pick_id << (3-ir)*8;
        tmp = tmp >> 24;
        widget->pick_color->red = (unsigned char) tmp;
        tmp = widget->pick_id << (3-ig)*8;
        tmp = tmp >> 24;
        widget->pick_color->green = (unsigned char) tmp;
        tmp = widget->pick_id << (3-ib)*8;
        tmp = tmp >> 24;
        widget->pick_color->blue = (unsigned char) tmp;
        tmp = widget->pick_id << (3-ia)*8;
	//   tmp = tmp >> 24;
        widget->pick_color->alpha = 255;
}



void ei_toplevel_configure(ei_widget_t* widget,
                           ei_size_t* requested_size,
                           ei_color_t* color,
                           int* border_width,
                           char** title,
                           ei_bool_t* closable,
                           ei_axis_set_t* resizable,
                           ei_size_t** min_size)
{
        ei_toplevel_t *toplevel = (ei_toplevel_t *)widget;
        if ( requested_size == NULL) {
                widget->requested_size.width = 320;
                widget->requested_size.height = 240;
              
        } else {
		widget->requested_size = *requested_size; 
        }
        
        if ( color == NULL) {
                toplevel->color = ei_default_background_color;
        } else {
                toplevel->color = *color;
        }
        if ( border_width == NULL) {
                toplevel->border_width = 4;
        } else {
                toplevel->border_width = *border_width;
        }
        if (title == NULL) {
                toplevel->title = "Toplevel";
        } else {
                toplevel->title = *title;
        }
        if ( closable == NULL) {
                toplevel->closable = EI_TRUE;
        } else {
                toplevel->closable = *closable;
        }
        if ( resizable == NULL) {
                toplevel->resizable = ei_axis_both;
        } else {
                toplevel->resizable = *resizable;
        }
        if ( min_size == NULL) {
                toplevel->min_size->width = 160;
                toplevel->min_size->height = 120;
        } else {
                toplevel->min_size = *min_size;
        }
        
        int ir, ig, ib, ia;
        hw_surface_get_channel_indices(surface_offscreen, &ir, &ig, &ib, &ia);
        
  
        uint32_t tmp = 0;
        tmp = widget->pick_id << (3-ir)*8;
        tmp = tmp >> 24;
        widget->pick_color->red = (unsigned char) tmp;
        tmp = widget->pick_id << (3-ig)*8;
        tmp = tmp >> 24;
        widget->pick_color->green = (unsigned char) tmp;
        tmp = widget->pick_id << (3-ib)*8;
        tmp = tmp >> 24;
        widget->pick_color->blue = (unsigned char) tmp;
        tmp = widget->pick_id << (3-ia)*8;
	//   tmp = tmp >> 24;
        widget->pick_color->alpha = 255;

/*

  if(toplevel->closable){
  // doit etre cree dans configure puisque potentiellement inexistant
  ei_widget_t* closing_button = ei_widget_create("button", widget);
  //parametres pour l'aspect du widget button
  ei_color_t red = {0xFF, 0x00, 0x00, 0xFF};
  int border = 2;
  int zero = 0;
  char* xshape = "X";
  ei_button_configure(closing_button, 
  NULL, &red, &border, &zero, NULL, 
  &xshape, NULL, NULL, NULL, 
  NULL, NULL, NULL, 
  NULL, NULL);
  // parametres pour le cote geometrie
  ei_anchor_t northwest = ei_anc_northwest;
  int button_x = 10;
  int button_y = 10;
  float button_rx = 1.0;
  float button_ry = 1.0;
  float button_rw =0.5;
  float button_rh =0.5;
  ei_place(closing_button, &northwest, 
  &button_x, &button_y, 
  NULL, NULL, 
  NULL, NULL, 
  NULL, NULL);
  }
*/
	/* if(toplevel->resizable != ei_axis_none){ */
	/* 	// meme chose */
	/* 	ei_widget_t* resizing_frame = ei_widget_create("frame", widget); */

	/* 	ei_size_t req_size = {15,15}; */
	/* 	ei_frame_configure(resizing_frame,  */
	/* 			   &req_size, color, NULL, NULL,  */
	/* 			   NULL, NULL, NULL, NULL,  */
	/* 			   NULL, NULL, NULL); */
		
	/* 	ei_anchor_t southeast = ei_anc_southeast; */
	/* 	float resizing_rx = 1.0; */
	/* 	float resizing_ry = 1.0; */
	/* 	ei_place(resizing_frame, &southeast, */
	/* 		 NULL, NULL, */
	/* 		 NULL, NULL, */
	/* 		 &resizing_rx, &resizing_ry, */
	/* 		 NULL, NULL); */
	/* } */


}

