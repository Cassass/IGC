#include "ei_geometrymanager.h"
#include "ei_placer.h"
#include "ei_types.h"
#include "ei_widget_toplevel.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

struct ei_geometrymanager_t *managers;

void ei_geometrymanager_register(ei_geometrymanager_t* geometrymanager)
{
        if (managers == NULL) {
                managers = geometrymanager;
                geometrymanager->next = NULL;
        } else {
                geometrymanager->next = managers;
                managers = geometrymanager;
        }

}

ei_geometrymanager_t* ei_geometrymanager_from_name(ei_geometrymanager_name_t name)
{
        if (strncmp((char*) name, "placeur", 7) == 0) {
                return manager_placeur;
        } else {
                return NULL; // a continuer si on veut d'autre "manager"
        }
}

void ei_geometrymanager_unmap(ei_widget_t* widget)
{
	widget->geom_params = NULL;
}





void ei_register_placer_manager()
{
        manager_placeur = calloc(1, sizeof (struct ei_geometrymanager_t));
        if (manager_placeur == NULL) {
                printf("manager = null\n");
                exit(EXIT_FAILURE);
        }
        strcpy(manager_placeur->name, "manager");
        manager_placeur->runfunc = runfunc_placer;
        manager_placeur->releasefunc = releasefunc_placer;
}
/////////////////////////FONCTIONS DE FACTORIASATION////////////////////////////

/*
  get_relative:
  prends en argument:
  -un pointeur sur la dimension relative fournie (éventuellement nul)
  -un widget
  -un string qui definie la dimension traitée
  renvoie:
  -un float contenant la dimension relative
  par defaut si le pointeur  n'est pas nul, renvoie la valeur qu'il pointe

  CAS PARTICULIERS :
  -si le pointeur est nul:
  prends en compte le fait que le widget n'ai pas de requested_size,
  dans ce cas, la dimension du widget sera celle du parent
  (en donnant 1.0 comme valeur relative vis a vis de son parnet)
*/
static float get_relative(float *relative,
			  ei_widget_t *widget,
			  char dimension[10])
{
        if (relative != NULL) {
                return *relative;
        } else if (strncmp(dimension, "width", 5) == 0) {
                if (widget->requested_size.width == 0 ||
		    widget->requested_size.width == 0) {
                        return 1.0;
                } else {
                        return 0.0;
                }
        } else if (strncmp(dimension, "height", 6) == 0) {
                if (widget->requested_size.height == 0 ||
		    widget->requested_size.height == 0) {
                        return 1.0;
                } else {
                        return 0.0;
                }
        } else {
                printf("failure\n");
                exit(EXIT_FAILURE);
        }
}

/*
  get_absolute:
  prends en argument:
  -un pointeur sur la dimension absolue fournie (eventuellement nul)
  -un widget
  -un string qui definie la dimension traitée
  -la valeur de la meme dimension relative
  renvoie :
  -un int contenant la dimension absolue
  par defaut si le pointeur n'est pas nul, renvoie la valeur qu'il pointe

  CAS PARTICULIERS :
  -si le pointeur est nul:
  prends en compte le fait que le widget n'ai pas de requested_size,
  dans ce cas, puisque get_relative donne au widget la taille de son parent,
  il n'est pas nécéssaire de donner de dimension absolue

  -sinon: On veut que le widget soit au moins de la taille de sa requested_size .
  On différencie le cas ou la taille relative suffit
  (dans ce cas , on retourne 0)
  et celui ou elle ne suffit pas
  (on retourne alors le minimum pour que la taille du widget soit de
  sa requised-size)
*/
static int get_absolute(int *absolute, ei_widget_t *widget, char dimension[10],
                        float relative)
{
        if (absolute != NULL)  {
                return *absolute;
        } else if (widget->requested_size.width == 0 ||
                   widget->requested_size.height == 0) {
                return 0;
        } else if (strncmp(dimension, "width", 5) == 0) {
                if (widget->requested_size.width < relative * (widget->parent->requested_size.width)) {
                        return 0;
                } else {
                        return widget->requested_size.width - relative * (widget->parent->requested_size.width);
                }
        } else if (strncmp(dimension, "height", 6) == 0) {
                if (widget->requested_size.height < relative * (widget->parent->requested_size.height)) {
                        return 0;
                } else {
                        return widget->requested_size.height - relative * (widget->parent->requested_size.height);
                }
        } else  {
		printf("failure\n");
		exit(EXIT_FAILURE);
	}
}

/*
  get_float:
  renvoie la valeur pointée pas flottant si celui_ci n'est pas nul,
  sinon renvoie 0.0
*/

static float get_float(float *flottant)
{
	if (flottant != NULL) {
		return *flottant;
	} else {
		return 0.0;
	}
}
/*
  get_int:
  renvoie la valeur pointée pas entier si celui_ci n'est pas nul,
  sinon renvoie 0
*/
static int get_int(int *entier)
{
	if (entier != NULL) {
		return *entier;
	} else {
		return 0;
	}
}

/*
  corner_coordinates_updates:
  soustrait a chaques valeures de coordonnees ( xet y relatives ou absolues)
  les constantes leur correspondant
  Utilisé pour le distinction de cas en fonction du switch sur l'anchor.
*/
static void corner_coordinates_updates(int *absolue_x,
				       int absolute_x_constant,
				       float *relative_x,
				       float relative_x_constant,
				       int *absolue_y,
				       int absolute_y_constant,
				       float *relative_y,
				       float relative_y_constant)
{
	*absolue_x -= absolute_x_constant;
	*relative_x -= relative_x_constant;
	*absolue_y -= absolute_y_constant;
	*relative_y -= relative_y_constant;
}


/////////////////////////////////DEBUT EI_PLACE/////////////////////////////////



void ei_place(ei_widget_t* widget, ei_anchor_t* anchor,
	      int* x, int* y, int* width, int* height,
	      float* rel_x, float* rel_y, float* rel_width,
	      float* rel_height)
{

	/* on commence par libérer le widget du geometrymanager qui s'occupait de */
	/*    lui jusqu'a présent si il exise, et quil nest pas placer  */
	/*    en appelant son champ relesefunc: */
	if     ((widget->geom_params != NULL) &&
		(widget->geom_params->manager != NULL) &&
		(widget->geom_params->manager->releasefunc != NULL) &&
		(strncmp((char*) widget->geom_params->manager->name,"placeur",7)!= 0)) {

		widget->geom_params->manager->releasefunc(widget);
	}


	/*creation du placeur correspondant au widget*/
	struct ei_placer_t *placer = calloc(1,sizeof(struct ei_placer_t));
	if (placer == NULL) {
		exit(EXIT_FAILURE);
	}


	/*configuration des attributs du placer*/
	placer->manager = *manager_placeur;

	/*initialisation des stings : char_width et char_height,
	  utilisés pour la factoriasation des fonctions*/
	char char_width[10];
	char char_height[10];
	strcpy(char_width, "width");
	strcpy(char_height,"height");

	/*traitements des largeurs et longueurs */

	/*relatives : */

	float relative_width = get_relative (rel_width,widget,char_width);
	float relative_height = get_relative (rel_height,widget,char_height);
	placer->relative_size.width = relative_width;
	placer->relative_size.height =relative_height;



	/* longeurs et largeurs absolues: */

	int absolute_width  =get_absolute(width,widget,char_width,relative_width);
	int absolute_height =get_absolute(height,widget,char_height,relative_height);


	placer->absolute_size.width = absolute_width;
	placer->absolute_size.height = absolute_height;




	/*on calcule maintenant x,y relatifs et absolus*/


	float relative_x = get_float (rel_x);
	float relative_y = get_float (rel_y);
	int absolute_x = get_int (x);
	int absolute_y = get_int (y);

	/*il reste a determiner a patir:
	  des coordonnées et tailles relatives et absolues
	  celles correspondantes au coin en haut a gauche du widget.
	  en tfaisant un switch sur l'anchor qui détermine quel
	  coin est désigné par ces coordonnées*/
	float *x_relative_corner = &relative_x;
	float *y_relative_corner = &relative_y;
	int *x_absolute_corner = &absolute_x;
	int *y_absolute_corner = &absolute_y;


	/*si lancrage est null,
	  les coordonnées sont deja bien initialisées
	  (par défaut il est deja sur le coin superieur gauche du widget);*/

	if (anchor != NULL) {
		switch (*anchor) {
			/*par défaut, si anchor=0,
			  l'ancrage est sur le coin NORD-OUEST.
			  Donc les voordonnées sont deja initialisées*/
		case 0:
		case 9:
			break;
		case 1:
			corner_coordinates_updates(x_absolute_corner,
						   floor(absolute_width / 2),
						   x_relative_corner,
						   relative_width / 2,
						   y_absolute_corner,
						   floor(absolute_height / 2),
						   y_relative_corner,
						   relative_height / 2 );
			break;
		case 2:
			corner_coordinates_updates(x_absolute_corner,
						   floor(absolute_width / 2),
						   x_relative_corner,
						   relative_width / 2,
						   y_absolute_corner,
						   0,
						   y_relative_corner,
						   0.0 );
			break;
		case 3:
			corner_coordinates_updates(x_absolute_corner,
						   absolute_width,
						   x_relative_corner,
						   relative_width,
						   y_absolute_corner,
						   0,
						   y_relative_corner,
						   0.0 );
			break;
		case 4:
			corner_coordinates_updates(x_absolute_corner,
						   absolute_width,
						   x_relative_corner,
						   relative_width,
						   y_absolute_corner,
						   floor(absolute_height / 2),
						   y_relative_corner,
						   relative_height / 2 );
			break;
		case 5:
			corner_coordinates_updates(x_absolute_corner,
						   absolute_width,
						   x_relative_corner,
						   relative_width,
						   y_absolute_corner,
						   absolute_height,
						   y_relative_corner,
						   relative_height);
			break;
		case 6:
			corner_coordinates_updates(x_absolute_corner,
						   floor(absolute_width / 2),
						   x_relative_corner,
						   relative_width / 2,
						   y_absolute_corner,
						   absolute_height,
						   y_relative_corner,
						   relative_height);
			break;
		case 7:
			corner_coordinates_updates(x_absolute_corner,
						   0,
						   x_relative_corner,
						   0.0,
						   y_absolute_corner,
						   absolute_height,
						   y_relative_corner,
						   relative_height);
			break;
		case 8:
			corner_coordinates_updates(x_absolute_corner,
						   0,
						   x_relative_corner,
						   0.0,
						   y_absolute_corner,
						   floor(absolute_height / 2),
						   y_relative_corner,
						   relative_height / 2);
			break;
		default:
			printf("Erreur sur l'ancrage\n");
			exit(EXIT_FAILURE);
			break;
		}
	}

	/*on mets les valeurs de x,y relatifs,absolus dans le placer.*/
	placer->relative_top_left_corner.x = *x_relative_corner;
	placer->relative_top_left_corner.y = *y_relative_corner;
	placer->absolute_top_left_corner.x = *x_absolute_corner;
	placer->absolute_top_left_corner.y = *y_absolute_corner;


	/*on met finalement le placer dans le champs geometry_param_t du widget*/
	widget->geom_params = (struct ei_geometry_param_t *) placer;
        runfunc_placer(widget);


        /////A VOIR : SI TOP LEVEL : appel de ei_place sur les 2 BOUTONS SI closable et resizable
        if (strncmp(widget->wclass->name, "toplevel", 8) == 0) {
                ei_toplevel_t *toplevel = (ei_toplevel_t*) widget;
		// configuration et dessin de la barre de deplacement
		ei_widget_t* top_frame = ei_widget_create("frame", widget);
		// parametres de configuration
		int zero = 0;

		ei_frame_configure(top_frame,
				   NULL, &(toplevel->color), &zero, NULL,
				   &(toplevel->title), NULL, NULL, NULL,
				   NULL, NULL, NULL);
		// parametres de dessin
		ei_anchor_t northwest = ei_anc_northwest;
		int text_h = 0;
		int text_w = 0;
		hw_text_compute_size(toplevel->title, ei_default_font, &text_h, &text_w);
		text_h /=4;
		ei_place(top_frame, &northwest,
			 &zero, &zero,
			 &(toplevel->wtoplevel.requested_size.width), &text_h,
			 NULL, NULL,
			 NULL, NULL);
		if (toplevel->closable == EI_TRUE){
			ei_widget_t* closing_button = ei_widget_create("button", widget);
			//parametres pour l'aspect du widget button
			ei_color_t red = {0xFF, 0x00, 0x00, 0xFF};
			int un = 1;
			char* xshape = "x";
			ei_button_configure(closing_button,
					    NULL, &red, &un, &zero, NULL,
					    &xshape, NULL, NULL, NULL,
					    NULL, NULL, NULL,
					    NULL, NULL);
			// parametres pour le cote geometrie
			int button_x = 1;
			int button_y = 1;
			int button_h = 20;
			int button_w = 20;
			ei_place(closing_button, &northwest,
				 &button_x, &button_y,
				 &button_w, &button_h,
				 NULL, NULL,
				 NULL, NULL);
		}
                if (toplevel->resizable != ei_axis_none) {
			ei_widget_t* resizing_button = ei_widget_create("button", widget);
			// parametres de configuration
			ei_size_t req_size = {15,15};
			ei_button_configure(resizing_button,
					    &req_size, &(toplevel->color), NULL, &zero, &zero,
					   NULL, NULL, NULL, NULL,
					    NULL, NULL, NULL, NULL, NULL);
			// parametres de placement
			ei_anchor_t southeast = ei_anc_southeast;
			float resizing_rx = 1.0;
			float resizing_ry = 1.0;
			ei_place(resizing_button, &southeast,
				 NULL, NULL,
				 NULL, NULL,
				 &resizing_rx, &resizing_ry,
				 NULL, NULL);
                }

        }
}


/*puis on appelle, finalement run_func de placer qui va mettre a
  jour les coordonnées du widget, grace aux dépendances mémorisées
  dans le placer*/


//////////////////////////////FIN EI_PLACER/////////////////////////////////////
