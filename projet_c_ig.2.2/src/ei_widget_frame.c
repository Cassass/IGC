#include "ei_widget_frame.h"
#include <stdio.h>
#include <string.h>


void* alloc_frame() {
	ei_frame_t* frame = calloc(1, sizeof (struct ei_frame_t));
//	frame->text = calloc(1, sizeof(char));
	frame->img_rect = calloc(1, sizeof(ei_rect_t));
	return frame;
//	return calloc(1, sizeof (struct ei_frame_t));
}

void release_frame(ei_widget_t* widget) {
//        printf("debut release_frame\n");
	//verifie si le widget appele appartient a la bonne classe
	if(widget->wclass != frame_class){
		printf("appel fct frame avec autre classe de widget\n");
		exit(EXIT_FAILURE);
	}
	ei_frame_t* frame = (ei_frame_t*) widget;
	//desallocation des champs propres a la classe frame
//	free(frame->text);
//        printf("avant frame->img_rect\n");
	/* if (frame->img_rect == NULL) { */
        /*         printf("cest null\n"); */
        /* } */
        free(frame->img_rect);
	//desallocation de la frame
//        printf("aprés frame->img_rect et avant frame\n");
        free(frame);
//        printf("fin release_frame\n");
}

// procedure d'insertion en queue classique
void insert_q_int(ei_linked_point_t** list, int x, int y)
{
	ei_linked_point_t* new = malloc(sizeof(struct ei_linked_point_t));
	ei_linked_point_t* current = *list;
	new->point.x = x;
	new->point.y = y;
	new->next = NULL;
        // si la liste est nulle on insere en tete
        if (*list == NULL) {
		*list = new;
        } else {
                // sinon on insere a la fin du dernier element
                //recherche du dernier elt
                while (current->next != NULL) {
                        current = current->next;
                }
                //on alloue le nouveau point chainé
                current->next = new;
        }
}

//liberation de la memoire allouee
void free_list(ei_linked_point_t** list)
{
	ei_linked_point_t* next = *list;
	while(next != NULL){
		next = (*list)->next;
		free(*list);
		*list = next;
	}
}

// retourne une couleur avec transparence vis a vis de la surface actuelle
ei_color_t ei_alpha_handling(ei_surface_t surface, ei_color_t top_color, 
			     ei_point_t pixel_coords, int offset)
{
	// recup des indices des couleurs
	int ir, ig, ib, ia;
	hw_surface_get_channel_indices(surface, &ir, &ig, &ib, &ia);
	// couleur de retour
	ei_color_t resulting_color = {0, 0, 0, top_color.alpha};
	//couleur de la surface
	ei_color_t surface_color;
	uint8_t* where = hw_surface_get_buffer(surface); //where <=> (0,0)
	// where est deplace a la case memoire liee aux coords du pixel choisi
	where += 4*(pixel_coords.y*offset + pixel_coords.x);
	surface_color.red = *(where + ir);
	surface_color.green = *(where + ig);
	surface_color.blue = *(where + ib);
	surface_color.alpha = *(where + ia);
	resulting_color.red = (top_color.alpha*top_color.red +
			       (255-top_color.alpha)*surface_color.red)/255;
	resulting_color.green = (top_color.alpha*top_color.green + (255-top_color.alpha)*surface_color.green)/255;
	resulting_color.blue =  (top_color.alpha*top_color.blue + (255-top_color.alpha)*surface_color.blue)/255;

	return resulting_color;
}

// dessine une surface simple, sans gestion de relief
void ei_draw_simple(ei_surface_t surface, 
		    ei_frame_t* frame, 
		    ei_rect_t* clipper, 
		    ei_bool_t est_clip)
{
        ei_linked_point_t** to_draw = malloc(sizeof(struct ei_linked_point_t*));
	*to_draw = NULL;
	//gestion de la transparence
	ei_color_t drawing_color = frame->color;
	if (hw_surface_has_alpha(surface) == EI_TRUE){
		// "premier" point a la couleur sous le widget a dessiner
		ei_point_t sample_point = 
			{frame->wframe.screen_location.top_left.x+ frame->border_width, 
			frame->wframe.screen_location.top_left.y+frame->border_width};
		drawing_color = ei_alpha_handling(surface, drawing_color, sample_point, 
						  frame->wframe.screen_location.size.width);
	}
        // top left corner
        insert_q_int(to_draw, 
		     frame->wframe.screen_location.top_left.x+frame->border_width,
		     frame->wframe.screen_location.top_left.y+frame->border_width);
        // top right corner
        insert_q_int(to_draw,
		     frame->wframe.screen_location.top_left.x+
		     frame->wframe.screen_location.size.width-
		     frame->border_width,
		     frame->wframe.screen_location.top_left.y+frame->border_width);
        //bottom right corner
        insert_q_int(to_draw, 
		     frame->wframe.screen_location.top_left.x+
		     frame->wframe.screen_location.size.width-
		     frame->border_width, 
		     frame->wframe.screen_location.top_left.y+
		     frame->wframe.screen_location.size.height-frame->border_width);
        // bottom left corner
        insert_q_int(to_draw,
		     frame->wframe.screen_location.top_left.x+frame->border_width, 
		     frame->wframe.screen_location.top_left.y+
		     frame->wframe.screen_location.size.height-frame->border_width);
	//dessin avec primitives fournies
	if(est_clip == EI_FALSE){
		ei_draw_polygon(surface, *to_draw, drawing_color, clipper);	      
	}else{
		ei_draw_polygon(surface, *to_draw, *(frame->wframe.pick_color), clipper);
	}
	free_list(to_draw);
}

//dessin de la partie superieure de la frame
void ei_draw_upper(ei_surface_t surface, 
		   ei_frame_t* frame, 
		   ei_rect_t* clipper, 
		   ei_bool_t lighter)
{
	ei_linked_point_t** to_draw = malloc(sizeof(ei_linked_point_t*));
	*to_draw = NULL;
	ei_color_t upper_color;
	upper_color.red = 0x00;
	upper_color.green = 0x00;
	upper_color.blue = 0x00;

	//calcul sur la couleur pour la gestion du relief
	if (lighter == EI_TRUE){
		upper_color.red = frame->color.red*0.3 + 255*0.7;
		upper_color.green = frame->color.green*0.3 + 255*0.7;
		upper_color.blue = frame->color.blue*0.3 + 255*0.7;
	}else{	
		upper_color.red = frame->color.red*0.7;
		upper_color.green = frame->color.green*0.7;
		upper_color.blue = frame->color.blue*0.7;
	}
	// gestion de la transparence
	ei_color_t drawing_color = upper_color;
	if (hw_surface_has_alpha(surface) == EI_TRUE){
		// "premier" point a la couleur sous le widget a dessiner
		ei_point_t sample_point = 
			{frame->wframe.screen_location.top_left.x, 
			 frame->wframe.screen_location.top_left.y};
		drawing_color = ei_alpha_handling(surface, drawing_color, sample_point, 
						  frame->wframe.screen_location.size.width);
	}	
	//bottom left corner
	insert_q_int(to_draw, 
		     frame->wframe.screen_location.top_left.x,
		     frame->wframe.screen_location.top_left.y+
		     frame->wframe.screen_location.size.height);
	//diag, 1e partie
	insert_q_int(to_draw, 
		     frame->wframe.screen_location.top_left.x+
		     frame->wframe.screen_location.size.height/2,
		     frame->wframe.screen_location.top_left.y+
		     frame->wframe.screen_location.size.height/2);
	//line
	insert_q_int(to_draw, 
		     frame->wframe.screen_location.top_left.x+
		     frame->wframe.screen_location.size.width-
		     frame->wframe.screen_location.size.height/2,
		     frame->wframe.screen_location.top_left.y+
		     frame->wframe.screen_location.size.height/2);
	//top right corner
	insert_q_int(to_draw, 
		     frame->wframe.screen_location.top_left.x+
		     frame->wframe.screen_location.size.width,
		     frame->wframe.screen_location.top_left.y);
	//top left corner
	insert_q_int(to_draw,
		     frame->wframe.screen_location.top_left.x,
		     frame->wframe.screen_location.top_left.y);
	//dessin
	ei_draw_polygon(surface, *to_draw, drawing_color, clipper);
	//liberation liste apres dessin
	free_list(to_draw);
}

//dessin de la partie inferieure de la frame
void ei_draw_lower(ei_surface_t surface, 
		   ei_frame_t* frame, 
		   ei_rect_t* clipper, 
		   ei_bool_t lighter)
{
	ei_linked_point_t** to_draw = malloc(sizeof(ei_linked_point_t*));
	*to_draw = NULL;
	ei_color_t lower_color;
	lower_color.red = 0x00;
	lower_color.blue = 0x00;
	lower_color.green = 0x00;
	lower_color.alpha = 0xFF;

	//calcul sur la couleur pour la gestion du relief
	if (lighter == EI_TRUE){
		lower_color.red = frame->color.red*0.3 + 255*0.7;
		lower_color.green = frame->color.green*0.3 + 255*0.7;
		lower_color.blue = frame->color.blue*0.3 + 255*0.7;
	}else{	
		lower_color.red = frame->color.red*0.7;
		lower_color.green = frame->color.green*0.7;
		lower_color.blue = frame->color.blue*0.7;
	}
	
	// gestion de la transparence
	ei_color_t drawing_color = lower_color;
	if (hw_surface_has_alpha(surface) == EI_TRUE){
		// point a la couleur sous le widget a dessiner
		ei_point_t sample_point = 
			{frame->wframe.screen_location.top_left.x, 
			 frame->wframe.screen_location.top_left.y+
			 frame->wframe.screen_location.size.height};
		drawing_color = ei_alpha_handling(surface, drawing_color, sample_point, 
						  frame->wframe.screen_location.size.width);
	}	
	//bottom left corner
	insert_q_int(to_draw, 
		     frame->wframe.screen_location.top_left.x,
		     frame->wframe.screen_location.top_left.y+
		     frame->wframe.screen_location.size.height);
	//diag, 1e partie
	insert_q_int(to_draw, 
		     frame->wframe.screen_location.top_left.x+
		     frame->wframe.screen_location.size.height/2,
		     frame->wframe.screen_location.top_left.y+
		     frame->wframe.screen_location.size.height/2);
	//line
	insert_q_int(to_draw, 
		     frame->wframe.screen_location.top_left.x+
		     frame->wframe.screen_location.size.width-
		     frame->wframe.screen_location.size.height/2,
		     frame->wframe.screen_location.top_left.y+
		     frame->wframe.screen_location.size.height/2);
	//top right corner
	insert_q_int(to_draw, 
		     frame->wframe.screen_location.top_left.x+
		     frame->wframe.screen_location.size.width,
		     frame->wframe.screen_location.top_left.y);
	//bottom right corner
	insert_q_int(to_draw,
		     frame->wframe.screen_location.top_left.x+
		     frame->wframe.screen_location.size.width,
		     frame->wframe.screen_location.top_left.y+
		     frame->wframe.screen_location.size.height);
	//dessin
	ei_draw_polygon(surface, *to_draw, drawing_color, clipper);
	//liberation liste apres dessin
	free_list(to_draw);
}

// dessin du texte dans une frame
void text_draw_frame(ei_surface_t surface, ei_frame_t* frame,  ei_rect_t* clipper)
{
	ei_point_t where = {0,0};
	int text_h = 0;
	int text_w = 0;
	hw_text_compute_size(frame->text, frame->text_font, &text_w, &text_h);
	if (frame->text_anchor == ei_anc_none || frame->text_anchor == ei_anc_center ||
	    frame->text_anchor == ei_anc_south || frame->text_anchor == ei_anc_north){
		where.x = frame->wframe.screen_location.top_left.x+
			(frame->wframe.screen_location.size.width-text_w)/2;
	}else if (frame->text_anchor == ei_anc_southeast ||frame->text_anchor == ei_anc_northeast ||
		  frame->text_anchor == ei_anc_east){
		where.x = frame->wframe.screen_location.top_left.x+
			frame->wframe.screen_location.size.width-text_w; //a droite
	}else{
		where.x = frame->wframe.screen_location.top_left.x; //a gauche
	}

	// ... et sur y...
	if (frame->text_anchor == ei_anc_none || frame->text_anchor == ei_anc_center ||
	    frame->text_anchor == ei_anc_east || frame->text_anchor == ei_anc_west){
		where.y = frame->wframe.screen_location.top_left.y+
			(frame->wframe.screen_location.size.height-text_h)/2;
	}else if (frame->text_anchor == ei_anc_southeast || 
		  frame->text_anchor == ei_anc_southwest ||
		  frame->text_anchor == ei_anc_south){
		where.y = frame->wframe.screen_location.top_left.y+
			frame->wframe.screen_location.size.height-text_h; //en bas
	}else{
		where.y = frame->wframe.screen_location.top_left.y; //en haut
	}
	ei_draw_text(surface, &where, 
		     frame->text, frame->text_font, &(frame->text_color), clipper);
} 

//partie creation de la liste chainee pour creation des rectangles
void ei_frame_draw(struct ei_widget_t *widget, ei_surface_t surface, ei_rect_t *clipper, ei_bool_t est_pick) 
{
	if(widget->wclass != frame_class){
		printf("appel fct frame avec autre classe de widget\n");
		exit(EXIT_FAILURE);
	}

        ei_frame_t* frame = (ei_frame_t*) widget;
        //gestion du cas où clipper est NULL: 
        //on limite donc le dessin à la taille de l'écran
        ei_rect_t* clipper_draw = malloc(sizeof (ei_rect_t));
        if (clipper == NULL) {
                *clipper_draw = hw_surface_get_rect(surface);
        } else {
                *clipper_draw = *clipper;
        }
        if (est_pick == EI_FALSE) {
		if (frame->relief == ei_relief_none){
			ei_draw_simple(surface, frame, clipper_draw, est_pick);
		}else if (frame->relief == ei_relief_raised){
			// si pas de border_width, pas de relief
			if (frame->border_width == 0)
				frame->border_width = 5;
			ei_draw_upper(surface, frame, clipper, EI_TRUE);
			ei_draw_lower(surface, frame, clipper, EI_FALSE);
			ei_draw_simple(surface, frame, clipper_draw, est_pick);
		}else{ // i.e relief sunken
			if (frame->border_width == 0)
				frame->border_width = 5;
			ei_draw_upper(surface, frame, clipper, EI_FALSE);
			ei_draw_lower(surface, frame, clipper, EI_TRUE);
			ei_draw_simple(surface, frame, clipper_draw, est_pick);
		}
        } else {
		ei_draw_simple(surface, frame, clipper_draw, est_pick);
	}
	if(frame->text != NULL){
		text_draw_frame(surface, frame, clipper_draw);
	}
	free(clipper_draw);
}

// partie dessin, s'appuie sur ei_frame_draw
void draw_frame(struct ei_widget_t *widget, ei_surface_t surface, ei_surface_t pick_surface,
		ei_rect_t *clipper) {
	if(widget->wclass != frame_class){
		printf("appel fct frame avec autre classe de widget\n");
		exit(EXIT_FAILURE);
	}
//        printf("début draw_frame\n");
        //dessin de la frame sur la surface utilisateur
        hw_surface_lock(surface);
        ei_frame_draw(widget, surface, clipper, EI_FALSE);        
	hw_surface_unlock(surface);
	hw_surface_update_rects(surface, NULL);
        //dessin de la frame sur la surface de picking1
        if (pick_surface != NULL) {
                hw_surface_lock(pick_surface);
//                printf("coucou\n");
                ei_frame_draw(widget, pick_surface, clipper, EI_TRUE);
                hw_surface_unlock(pick_surface);
        }	
//        printf("fin draw_frame\n");
}

// valeurs par défaut de la classe frame. 
//Ne prend pas en compte les parametres de la classe widget
void setdefaults_frame(struct ei_widget_t* widget) {
	if(widget->wclass != frame_class){
		printf("appel fct frame avec autre classe de widget\n");
		exit(EXIT_FAILURE);
	}
        ei_frame_t* frame = (ei_frame_t*) widget;

        frame->color = ei_default_background_color;
        frame->border_width = 0;
        frame->relief = ei_relief_none;
        frame->text = NULL;
        frame->text_font = ei_default_font;
        frame->text_color = ei_font_default_color;
        frame->text_anchor = ei_anc_center;
        frame->img = NULL;
        frame->img_rect = NULL;
        frame->img_anchor = ei_anc_center;
}

//change la place occupee par le widget suite modif dimensions
void geom_frame(struct ei_widget_t* widget, ei_rect_t rect) {
	if(widget->wclass != frame_class){
		printf("appel fct frame avec autre classe de widget\n");
		exit(EXIT_FAILURE);
	}
        //maj de la screen location a la taille de rect
	widget->screen_location = rect;
}
