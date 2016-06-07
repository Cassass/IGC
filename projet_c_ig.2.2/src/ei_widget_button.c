#include "ei_widget_button.h"
#include "ei_event.h"
#include "binding.h"

#include <stdlib.h>
#include <stdio.h>

void* alloc_button()
{
	ei_button_t* button = calloc(1, sizeof(struct ei_button_t));
        if (button == NULL) {
		printf ("button = null");
                exit(EXIT_FAILURE);
        }
	button->img_rect = calloc(1, sizeof(ei_rect_t));
	button->user_param = calloc(1, sizeof(void));
	int text_max_length = 20;
	button->text = calloc(text_max_length, sizeof(char));
        if (button->img_rect == NULL) {
		printf ("button->img_rect= null");
                exit(EXIT_FAILURE);
        }
	// on lie les évènements communs aux boutons:
	ei_bind (ei_ev_mouse_buttondown, NULL, "button", bouton_a_enfoncer, NULL);
	// A VERIFIER que le user_param doit etre a null ou pas?
        return button;

}

void release_button(ei_widget_t* widget)
{
	if(widget->wclass != button_class)
	{
		printf("utilisation fct button avec mauvaise classe\n");
	}
	ei_button_t* button = (ei_button_t*) widget;
	//desallocation de tous les parametres specifiques a la classe button
//	free(button->text);
	free(button->img_rect);
	free(button->user_param);
	//desallocation du button
	free(button);
}

//dessin pour gestion du relief
void light_draw(ei_surface_t surface, ei_button_t* button, 
		ei_linked_point_t* list, ei_rect_t* clipper)
{
	//calcul sur la couleur
	ei_color_t lighter_color;
	lighter_color.red = button->color.red*0.3 + 255*0.7;
	lighter_color.blue = button->color.blue*0.3 + 255*0.7;
	lighter_color.green = button->color.green*0.3 + 255*0.7;
	lighter_color.alpha = button->color.alpha;

	
	// gestion de la transparence
	ei_color_t drawing_color = lighter_color;
	if (hw_surface_has_alpha(surface) == EI_TRUE){
		// les points de la liste sont a la bonne couleur
		ei_point_t sample_point = {list->point.x, list->point.y};
		drawing_color = ei_alpha_handling(surface, drawing_color, sample_point, 
						  button->wbutton.screen_location.size.width);
	}
	
	//dessin en couleur plus claire
	ei_draw_polygon(surface, list, drawing_color, clipper);
}

//dessin pour gestion du relief
void dark_draw(ei_surface_t surface,
	       ei_button_t* button,
	       ei_linked_point_t* list,
	       ei_rect_t* clipper)
{
	//calcul sur la couleur
	ei_color_t darker_color;
	darker_color.red = button->color.red*0.7;
	darker_color.blue = button->color.blue*0.7;
	darker_color.green = button->color.green*0.7;
	darker_color.alpha = button->color.alpha;

	// gestion de la transparence
	ei_color_t drawing_color = darker_color;
	if (hw_surface_has_alpha(surface) == EI_TRUE){
		// les points de la liste sont a la bonne couleur
		ei_point_t sample_point = {list->point.x, list->point.y};
		drawing_color = ei_alpha_handling(surface, drawing_color, sample_point, 
						  button->wbutton.screen_location.size.width);
	}

	//dessin en couleur plus claire
	ei_draw_polygon(surface, list, drawing_color, clipper);
}

// dessin pour gestion du relief
void normal_draw(ei_surface_t surface,
		 ei_button_t* button,
		 ei_linked_point_t* list,
		 ei_rect_t* clipper)
{
	// gestion de la transparence
	ei_color_t drawing_color = button->color;
	if (hw_surface_has_alpha(surface) == EI_TRUE){
		// les points de la liste sont a la bonne couleur
		ei_point_t sample_point = {list->point.x, list->point.y};
		drawing_color = ei_alpha_handling(surface, drawing_color, sample_point, 
						  button->wbutton.screen_location.size.width);
	}
	
	ei_draw_polygon(surface, list, drawing_color, clipper);
}

// "reduction" d'un rectangle
ei_rect_t reduce(ei_rect_t rectangle, int factor)
{
	ei_rect_t reduced;
	reduced.top_left.x = rectangle.top_left.x+factor;
	reduced.top_left.y = rectangle.top_left.y+factor;
	reduced.size.width = rectangle.size.width - 2*factor;
	reduced.size.height = rectangle.size.height - 2*factor;
	return reduced;
}

// fct affiliee au widget button
void draw_button(struct ei_widget_t *widget,
		 ei_surface_t surface,
		 ei_surface_t pick_surface,
		 ei_rect_t *clipper)
{
	if(widget->wclass != button_class)
	{
		printf("utilisation fct button avec mauvaise classe\n");
	}
	ei_button_t* button = (ei_button_t*) widget;
	// allocation et initialisation de la liste de points a tracer
	ei_linked_point_t** to_draw = malloc(sizeof(struct ei_linked_point_t*));
	*to_draw = NULL;
	// gestion du clipper NULL : on ne restreint le dessin qu'a la taille de l'ecran
	ei_rect_t* clipper_draw = malloc(sizeof(ei_rect_t));
	if (clipper == NULL){
		*clipper_draw = hw_surface_get_rect(surface);
	}else{
		*clipper_draw = *clipper;
	}
	// creation du rectangle de dessin
	// unlock de l'ecran ppal : debut du dessin
//	hw_surface_lock(surface);
	if (button->relief != ei_relief_none){
		//dessin partie haute
		*to_draw = ei_rounded_generate(button->wbutton.screen_location,
					       button->corner_radius, HI);
		if (button->relief == ei_relief_raised){
			light_draw(surface, button, *to_draw, clipper_draw);
		}else{
			dark_draw(surface, button, *to_draw, clipper_draw);
		}
		*to_draw = NULL; //probablement inutile

		//dessin partie basse
		*to_draw = ei_rounded_generate(button->wbutton.screen_location,
					       button->corner_radius, LO);
		if (button->relief == ei_relief_raised){
			dark_draw(surface, button, *to_draw, clipper_draw);
		} else{
			light_draw(surface, button, *to_draw, clipper_draw);
		}
		*to_draw = NULL;
		ei_rect_t reduc_rect = reduce(button->wbutton.screen_location,
					      button->border_width);
		/* //dessin du button REDUIT */
		*to_draw = ei_rounded_generate(reduc_rect,
					       button->corner_radius-button->border_width, ALL);
		normal_draw(surface, button, *to_draw, clipper_draw);
	}else{
		// pas de relief : dessin du button echelle 1:1
		*to_draw = ei_rounded_generate(button->wbutton.screen_location,
					       button->corner_radius, ALL);
		normal_draw(surface, button, *to_draw, clipper_draw);
	}

	// calcul de l'ancrage du texte...
	if (button->text != NULL){ // text can't be NULL
		int* text_width = malloc(sizeof(int));
		int* text_height = malloc(sizeof(int));
		ei_point_t* where = malloc(sizeof(ei_point_t));
		hw_text_compute_size(button->text, button->text_font, text_width, text_height);
		// ... sur x...
		if (button->text_anchor == ei_anc_none || button->text_anchor == ei_anc_center ||
		    button->text_anchor == ei_anc_south || button->text_anchor == ei_anc_north){
			where->x = button->wbutton.screen_location.top_left.x+
				(button->wbutton.screen_location.size.width-*text_width)/2;
		}else if (button->text_anchor == ei_anc_southeast ||button->text_anchor == ei_anc_northeast ||
			  button->text_anchor == ei_anc_east){
			where->x = button->wbutton.screen_location.top_left.x+
				button->wbutton.screen_location.size.width-*text_width; //a droite
		}else{
			where->x = button->wbutton.screen_location.top_left.x; //a gauche
		}

		// ... et sur y...
		if (button->text_anchor == ei_anc_none || button->text_anchor == ei_anc_center ||
		    button->text_anchor == ei_anc_east || button->text_anchor == ei_anc_west){
			where->y = button->wbutton.screen_location.top_left.y+
				(button->wbutton.screen_location.size.height-*text_height)/2;
		}else if (button->text_anchor == ei_anc_southeast || 
			  button->text_anchor == ei_anc_southwest ||
			  button->text_anchor == ei_anc_south){
			where->y = button->wbutton.screen_location.top_left.y+
				button->wbutton.screen_location.size.height-*text_height; //en bas
		}else{
			where->y = button->wbutton.screen_location.top_left.y; //en haut
		}


		// dessin du texte
		ei_draw_text(surface, where, button->text, button->text_font, &(button->text_color), clipper_draw);
	}
	//unlock de la surface : fin du dessin
//	hw_surface_unlock(surface);
	// meme chose pour la surface de picking
	if (pick_surface != NULL){
//		hw_surface_lock(pick_surface);
		ei_draw_polygon(pick_surface, *to_draw, (*button->wbutton.pick_color), clipper_draw);
//		hw_surface_unlock(pick_surface);
//		hw_surface_update_rects(pick_surface, NULL);
	}
	// update des rectangles
	//hw_surface_update_rects(surface, NULL);
}

void setdefaults_button(struct ei_widget_t* widget)
{
	if(widget->wclass != button_class)
	{
		printf("utilisation fct button avec mauvaise classe\n");
		exit(EXIT_FAILURE);
	}
        ei_button_t* button = (ei_button_t*) widget;
	button->color= ei_default_background_color;
	button->border_width = k_default_button_border_width;
	button->corner_radius = k_default_button_corner_radius;
	button->relief = ei_relief_raised;
	button->text = NULL;
	button->text_font = ei_default_font;
	button->text_color = ei_font_default_color;
	button->text_anchor = ei_anc_center;
	button->img = NULL;
	button->img_rect = NULL;
	button->img_anchor = ei_anc_center;
	button->callback = NULL;
	button->user_param = NULL;

}
void geom_button(struct ei_widget_t* widget, ei_rect_t rect)
{
	if(widget->wclass != button_class)
	{
		printf("utilisation fct button avec mauvaise classe\n");
		exit(EXIT_FAILURE);
	}
	
	widget->screen_location = rect;
}
