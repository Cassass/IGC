#include "ei_widget_toplevel.h"
#include "ei_types.h"

#include <stdlib.h>
#include <stdio.h>


void* alloc_toplevel()
{
	ei_toplevel_t* toplevel = calloc(1, sizeof(struct ei_toplevel_t));
	if (toplevel == NULL) {
		printf ("toplevel = null");
                exit(EXIT_FAILURE);
        }
	toplevel->min_size = calloc(1, sizeof(ei_size_t));
        if (toplevel->min_size == NULL) {
		printf ("toplevel->min_size = null");
                exit(EXIT_FAILURE);
        }
	int title_max_size = 20;
	toplevel->title = calloc(title_max_size, sizeof(char));
        
	// on lie les évènements aux toplevel:
	// uniquement pr le deplacement de la fenetre
        return toplevel;
	
}

void release_toplevel(ei_widget_t* widget)
{
	if(widget->wclass != toplevel_class)
	{
		printf("utilisation fct toplevel avec mauvaise classe\n");
	}
        ei_toplevel_t *toplevel = (ei_toplevel_t *)widget;
        free(toplevel->min_size);
        free(toplevel);
	
}

//genere une liste de points chaines pour le background du toplevel
void ei_top_gene(ei_linked_point_t** list, ei_toplevel_t* toplevel, int bar_size){
	// top left corner
        insert_q_int(list, 
		     toplevel->wtoplevel.screen_location.top_left.x,
		     toplevel->wtoplevel.screen_location.top_left.y);
        // top right corner
        insert_q_int(list,
		     toplevel->wtoplevel.screen_location.top_left.x+
		     toplevel->wtoplevel.screen_location.size.width,
		     toplevel->wtoplevel.screen_location.top_left.y);
        //bottom right corner
        insert_q_int(list, 
		     toplevel->wtoplevel.screen_location.top_left.x+
		     toplevel->wtoplevel.screen_location.size.width, 
		     toplevel->wtoplevel.screen_location.top_left.y + bar_size);
        // bottom left corner
        insert_q_int(list,
		     toplevel->wtoplevel.screen_location.top_left.x, 
		     toplevel->wtoplevel.screen_location.top_left.y + bar_size);

}

//genere une liste de points chaines pour le foreground du toplevel
void ei_fg_gene(ei_linked_point_t** list, ei_toplevel_t* toplevel, int offset, int bar_size){
	// top left corner
        insert_q_int(list, 
		     toplevel->wtoplevel.screen_location.top_left.x + offset,
		     toplevel->wtoplevel.screen_location.top_left.y + bar_size);
        // top right corner
        insert_q_int(list,
		     toplevel->wtoplevel.screen_location.top_left.x+
		     toplevel->wtoplevel.screen_location.size.width - offset,
		     toplevel->wtoplevel.screen_location.top_left.y + bar_size);
        //bottom right corner
        insert_q_int(list, 
		     toplevel->wtoplevel.screen_location.top_left.x+
		     toplevel->wtoplevel.screen_location.size.width - offset, 
		     toplevel->wtoplevel.screen_location.top_left.y+
		     toplevel->wtoplevel.screen_location.size.height-offset);
        // bottom left corner
        insert_q_int(list,
		     toplevel->wtoplevel.screen_location.top_left.x + offset, 
		     toplevel->wtoplevel.screen_location.top_left.y+
		     toplevel->wtoplevel.screen_location.size.height - offset);

}

//fonction de dessin de la classe toplevel
void draw_toplevel(struct ei_widget_t *widget, ei_surface_t surface, ei_surface_t pick_surface,
		   ei_rect_t *clipper)
{
	if(widget->wclass != toplevel_class)
	{
		printf("utilisation fct toplevel avec mauvaise classe\n");
	}
        ei_toplevel_t *toplevel = (ei_toplevel_t *)widget;
	// recuperation de la hauteur du texte pour la taille de la barre
	int title_size_height = 0;
	int title_size_width = 0;
	hw_text_compute_size(toplevel->title, 
			     ei_default_font, &title_size_width, &title_size_height);	
	ei_linked_point_t** to_draw = malloc(sizeof(struct ei_linked_point_t*));
	*to_draw = NULL;
	
	ei_rect_t* clipper_draw = malloc(sizeof(ei_rect_t));
	if (clipper != NULL){
		*clipper_draw = *clipper;
	}else{
		*clipper_draw = hw_surface_get_rect(surface);
	}
	// dessin du corps exterieur de la toplevel
	*to_draw = NULL;
	ei_fg_gene(to_draw, toplevel, 0, title_size_height);
	ei_draw_polygon(surface, *to_draw, toplevel->color, clipper_draw);
	// dessin du corps interieur de la toplevel
	*to_draw = NULL;
	ei_fg_gene(to_draw, toplevel, toplevel->border_width, title_size_height);
	ei_color_t lighter = {toplevel->color.red*0.6 + 255*0.4, 
			      toplevel->color.green*0.6 + 255*0.4,
			      toplevel->color.blue*0.6 + 255*0.4,
			      toplevel->color.alpha};
	ei_draw_polygon(surface, *to_draw, lighter, clipper_draw);
	// le dessin des autres composantes dependent du dessin des enfants (cf. ei_place.c)
}

void setdefaults_toplevel(struct ei_widget_t* widget)
{
        ei_toplevel_t *toplevel = (ei_toplevel_t *)widget;
        toplevel->color = ei_default_background_color;
        toplevel->border_width = 4;
        toplevel->title = "Toplevel";
        toplevel->closable = EI_TRUE;
        toplevel->resizable = ei_axis_both;
        toplevel->min_size->width = 160;
        toplevel->min_size->height = 120;
}

void geom_toplevel(struct ei_widget_t* widget, ei_rect_t rect)
{
	if (widget->wclass != toplevel_class){
		printf("appel fct toplevel avec autre classe de widget\n");
		exit(EXIT_FAILURE);	     
	}
	// maj de la screen location a la taille de rect
	widget->screen_location = rect;
}
