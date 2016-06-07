#include "ei_button_draw.h"

//fonction similaire a celle de ei_widget_frame.h pour l'insertion en queue
//autre solution : polymorphisme
void insert_q(ei_linked_point_t** list, float x, float y) {
	//printf("dans insert\n");
	ei_linked_point_t* new = malloc(sizeof(struct ei_linked_point_t));
	new->point.x = (int)x;
	new->point.y = (int)y;
	new->next = NULL;
	ei_linked_point_t* current = *list;

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
	//printf("fin insert\n");
}

// fusion entre deux listes
void fusion (ei_linked_point_t** list, ei_linked_point_t* list2)
{
	//printf("dans fusion");
	ei_linked_point_t* current = *list;
	// recherche du dernier elt
	if (list2 != NULL){
		if (*list != NULL){
			while (current->next != NULL){
				current = current->next;
			}
			// fusion avec la deuxieme liste
			current->next = list2;
		}else{
			*list = list2;
		}
	}
}

// cree la liste de points chaines pour un arc entre 2 pts donnes pas leurs angles
ei_linked_point_t* ei_arc_generate(ei_point_t center,int radius, float start_agl, float end_agl)
{
	int n = 150; //modifier pour obtenir un resultat +/- discretise

	// on liste n points , de celui a l'angle initial jusqu'a celui a l'angle final
	// on travaille en repere indirect, partir du ppe que tt est faux
	ei_linked_point_t** list = malloc(sizeof(struct ei_linked_point_t*));
	*list = NULL;
	if(list == NULL){
		printf("attention liste chainee init a NULL\n");
		exit(EXIT_FAILURE);	       
	}
	// calcul actuel :
	// pour x : xc + r*cos(ai + i*(af-ai)/n)
	// pour y : yc - r*sin(ai + i*(af-ai)/n)
	for (int i = 0; i <= n; i++){
		float xf = (float)center.x+(float)radius*cos(start_agl+(float)i*(end_agl-start_agl)/(float)n);
		float yf = (float)center.y+(float)radius*sin(start_agl+(float)i*(end_agl-start_agl)/(float)n);
		insert_q(list,xf,yf);
	}
	return *list;
}

//genere une liste de points dessinant un button
ei_linked_point_t* ei_rounded_generate(ei_rect_t rectangle, int radius, enum dessin zone)
{
	ei_linked_point_t** list = malloc(sizeof(struct ei_linked_point_t));
	*list = NULL;
	ei_linked_point_t* rotation = NULL;
	ei_point_t center;
	// si on veut dessiner la partie supérieure ou la totalité
	if (zone == HI){
		// upper half bottom left
		center.x = rectangle.top_left.x+radius;
		center.y = rectangle.top_left.y+rectangle.size.height-radius;
		rotation = ei_arc_generate(center, radius, 3*PI/4, PI);
		fusion(list, rotation);
		// top left
		center.x = rectangle.top_left.x+radius;
		center.y = rectangle.top_left.y+radius;
		rotation = ei_arc_generate(center, radius, PI, 3*PI/2);
		fusion(list, rotation);
		// upper half top right
		center.x = rectangle.top_left.x+rectangle.size.width-radius;
		center.y = rectangle.top_left.y+radius;
		rotation = ei_arc_generate(center,radius, 3*PI/2, 7*PI/4);
		fusion(list, rotation);

		//diag, premiere partie
		float x = (float)(rectangle.top_left.x+
				  rectangle.size.width-rectangle.size.height/2);
		float y = (float)(rectangle.top_left.y+rectangle.size.height/2);
		insert_q(list, x, y);
		//horizontal line
		x = (float)(rectangle.top_left.x+rectangle.size.height/2);
		y = (float)(rectangle.top_left.y+rectangle.size.height/2);
		insert_q(list, x, y);
		//diag, deuxieme partie
		x = (float)(rectangle.top_left.x+radius)+(float)radius*cos(5*PI/4);
		y = (float)(rectangle.top_left.y+rectangle.size.height-radius)-
			(float)radius*sin(5*PI/4);
		insert_q(list, x, y);
	}else if (zone == LO){
		// lower half top right
		center.x = rectangle.top_left.x+rectangle.size.width-radius;
		center.y = rectangle.top_left.y+radius;
		rotation = ei_arc_generate(center, radius, 7*PI/4, 2*PI);
		fusion(list, rotation);
		// bottom right
		center.x = rectangle.top_left.x+rectangle.size.width-radius;
		center.y = rectangle.top_left.y+rectangle.size.height-radius;
		rotation = ei_arc_generate(center, radius, 0, PI/2);
		fusion(list, rotation);
		// lower half bottom left
		center.x = rectangle.top_left.x+radius;
		center.y = rectangle.top_left.y+rectangle.size.height-radius;
		rotation = ei_arc_generate(center, radius, PI/2, 3*PI/4);
		fusion(list, rotation);

		// on dessine ici la diagonale dans les deux cas restants
		// premiere partie
		// x1 puisque x n'a peut etre pas ete defini (cas LO)
		float x1 = (float)(rectangle.top_left.x+rectangle.size.height/2);
		float y1 = (float)(rectangle.top_left.y+rectangle.size.height/2);
		insert_q(list, x1, y1);
		
		// horizontal line
		x1 = (float)(rectangle.top_left.x+rectangle.size.width-rectangle.size.height/2);
		y1 = (float)(rectangle.top_left.y+rectangle.size.height/2);
		insert_q(list, x1, y1);
		
		//deuxieme partie
		x1 = (float)(rectangle.top_left.x+rectangle.size.width-radius) +
			(float)radius*cos(PI/4);
		y1 = (float)(rectangle.top_left.y+radius) - (float)radius*sin(PI/4);
		insert_q(list, x1, y1);

	}else{ //i.e
		// bottom left
		center.x = rectangle.top_left.x+radius;
		center.y = rectangle.top_left.y+rectangle.size.height-radius;
		rotation = ei_arc_generate(center, radius, PI/2, PI);
		fusion(list, rotation);
		// top left
		center.x = rectangle.top_left.x+radius;
		center.y = rectangle.top_left.y+radius;
		rotation = ei_arc_generate(center, radius, PI, 3*PI/2);
		fusion(list, rotation);
		// top right
		center.x = rectangle.top_left.x+rectangle.size.width-radius;
		center.y = rectangle.top_left.y+radius;
		rotation = ei_arc_generate(center,radius, 3*PI/2, 2*PI);
		fusion(list, rotation);		
		// bottom right
		center.x = rectangle.top_left.x+rectangle.size.width-radius;
		center.y = rectangle.top_left.y+rectangle.size.height-radius;
		rotation = ei_arc_generate(center, radius, 0, PI/2);
		fusion(list, rotation);		
	}
	
	return *list;
}


