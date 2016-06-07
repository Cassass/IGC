#include "ei_application.h"
#include "ei_widgetclass.h"
#include "ei_widget.h"
#include "ei_widget_frame.h"
#include "ei_widget_button.h"
#include "ei_widget_toplevel.h"
#include "ei_geometrymanager.h"
#include "hw_interface.h"
#include "ei_event.h"
#include "ei_types.h"
#include "liberer.h"
#include "ei_placer.h"
#include "binding.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
//#include <stddef.h>

struct ei_frame_t *racine;
struct ei_surface_t *surface_racine;
bool sortie_boucle;
ei_linked_rect_t *rects_a_changer;


void dessin_env_rec(ei_widget_t* current){
	if (current != NULL){ // condition d'arret : rien a dessiner
		current->wclass->drawfunc(current, surface_racine, surface_offscreen, NULL);
		if (current->children_head == NULL){ // ie : fin du parcours en profondeur
			if (current->next_sibling != NULL){//ie : parcours en largeur pas fini
				dessin_env_rec(current->next_sibling);
			}
		}else{
			dessin_env_rec(current->children_head);
		}
	}
}
void dessin_environnement () 
{
/*
	printf("debut dessin environnement\n");
*/
	struct ei_widget_t *current = (ei_widget_t*)racine;
	dessin_env_rec(current);

        /* racine->wframe.wclass->drawfunc(&(racine->wframe), */
        /*       surface_racine, surface_offscreen, NULL); // A revoir le 3eme et le 4eme param? */
        /* struct ei_widget_t *cour = racine->wframe.children_head; */
        /* if (cour != NULL) { */
        /*         while (cour != racine->wframe.children_tail) { */
        /*                 cour->wclass->drawfunc(cour, surface_racine, */
        /*                         surface_offscreen, NULL); */
        /*                 cour = cour->next_sibling; */
        /*         } */
        /*         cour->wclass->drawfunc(cour, surface_racine, */
        /*                 surface_offscreen, NULL); */
        /* } */

/*
        racine->wframe.wclass->drawfunc(&(racine->wframe),
                surface_offscreen, surface_racine, NULL); // A revoir le 3eme et le 4eme param?
        struct ei_widget_t *cour = racine->wframe.children_head;
        if (cour != NULL) {
                while (cour != racine->wframe.children_tail) {
                        cour->wclass->drawfunc(cour, surface_offscreen,
                                surface_racine, NULL);
                        cour = cour->next_sibling;
                }
                cour->wclass->drawfunc(cour, surface_offscreen,
                        surface_racine, NULL);
        }
*/
/*
	printf("fin dessin environnement\n");
*/
}

void ei_app_create(ei_size_t* main_window_size, ei_bool_t fullscreen)
{
        hw_init();
        // création et enregistrement des classes:
        // frame:
        ei_frame_register_class();
        ei_widgetclass_register(frame_class);

        //toplevel: 
        ei_toplevel_register_class();
        ei_widgetclass_register(toplevel_class);

        //button:
        ei_button_register_class();
        ei_widgetclass_register(button_class);

        // création et enregistrement du gestionnaire de géométrie "placeur"
        ei_register_placer_manager();
        ei_geometrymanager_register(manager_placeur);

        // création du widget racine de la classe frame    
        struct ei_widget_t *mon_widget = ei_widget_create("frame", NULL);
        racine = (struct ei_frame_t*) mon_widget;
        racine->wframe.requested_size = *main_window_size;
        racine->wframe.screen_location.size = *main_window_size;
        racine->wframe.screen_location.top_left.x = 0;
        racine->wframe.screen_location.top_left.y = 0;
        surface_racine = hw_create_window(main_window_size,
                fullscreen);

        // création d’une surface offscreen pour la gestion du picking
        //!\\ ATTENTION !!! le booléen en param de cette fonction est peut etre à changer!!! 
        surface_offscreen = hw_surface_create(surface_racine, main_window_size, EI_TRUE);

        //printf("fin ei_app_create\n");
}

void ei_app_free()
{
        // libere tous les widgets
        app_free_recursive(ei_app_root_widget());

        // libere les classes
        free(frame_class);
        free(button_class);
        free(toplevel_class);

        // libere les gestionnaires de géométrie
        free(manager_placeur);

        // libere le "hardware"
        hw_quit();
}

void ei_app_run()
{
        mvt_souris = false;
        mvt_clavier = false; // a enlever
        etait_sur_bouton = false;
        sort_de_relacher_click=false;
        rects_a_changer = NULL;
        //printf("début app run\n");

        // on bloque la surface de l'écran et celle de picking:
        hw_surface_lock(surface_racine);
        hw_surface_lock(surface_offscreen);
        // on dessine l'environnement de départ
        dessin_environnement();
        // on débloque les surfaces:
        hw_surface_unlock(surface_racine);
        hw_surface_unlock(surface_offscreen);

        hw_surface_update_rects(surface_racine, NULL);


        //getchar();
        //printf("on commence a gerer les evenements\n");
        // on gère les évènements
        sortie_boucle = false;
/*
        printf("début gestion evenements\n");
*/
         
        while (!sortie_boucle) {

                // a chaque tour de de boucle il faut retirer de la liste binding les evenements dont le booléen supprime_moi est à true
                mise_a_jour_binding(binding);
               
                // on attend d'avoir un évenement à traiter
                struct ei_event_t *event = malloc(sizeof (struct ei_event_t));
                if (event == NULL) {
                        printf("event = null\n");
                        exit(EXIT_FAILURE);
                }


                bool traiter_evenement = false;
                while (traiter_evenement == false) {
                       ei_linked_rect_t *cour1 = rects_a_changer;
                                while (cour1 != NULL) {
                                        fprintf(stderr, "next=%p rect.topleft.x=%x rect.topleft.y=%x rect.size.height=%x\n", cour1->next, 
                                                cour1->rect.top_left.x, cour1->rect.top_left.y, cour1->rect.size.height);
                                        
                                        cour1 = cour1->next;
                                }
                        hw_event_wait_next(event);
                        cour1 = rects_a_changer;
                                while (cour1 != NULL) {
                                        fprintf(stderr,"next1=%p rect.topleft.x=%x rect.topleft.y=%x rect.size.height=%x\n", cour1->next, 
                                                cour1->rect.top_left.x, cour1->rect.top_left.y, cour1->rect.size.height);
                                        cour1 = cour1->next;
                                }
                        
                        if ((mvt_souris == true) && (event->type == ei_ev_mouse_move) &&
                                (widget_prec != ei_widget_pick(&event->param.mouse.where)) && (etait_sur_bouton)) {
                                traiter_evenement = true;
                        } else if ( (mvt_souris == true) && (event->type == ei_ev_mouse_move) &&
                                (widget_prec == ei_widget_pick(&event->param.mouse.where)) && (!etait_sur_bouton) ) {
                                traiter_evenement = true;
                        } else if ( (event->type == ei_ev_mouse_buttondown) && (strncmp(ei_widget_pick(&event->param.mouse.where)->wclass->name,"button",6)!=0)) {
                                while (event->type != ei_ev_mouse_buttonup){
                                        hw_event_wait_next(event);
                                }
                                traiter_evenement = false;
                        } else if ((event->type != ei_ev_mouse_move) && (event->type != 2 && event->type != 3)) {
                                
                                traiter_evenement = true;
                               
                                

                        }
                        
                }
                // on traite l'évènement:
                ei_bool_t arreter_appeler_callback = EI_FALSE;
                
                while (arreter_appeler_callback != EI_TRUE) {
                        
                        
/*
                        printf("nouveau callback\n");
*/
                        mise_a_jour_binding(binding);
                        struct info_event *cour = binding[event->type];
                        if (cour != NULL) {
/*
                                printf("il y a un lien pr cet evenement\n");
*/
                                struct ei_widget_t *widget = NULL;
                                // si l'evenement est en rapport la souris
                                if (event->type == ei_ev_mouse_buttondown) {
                                        printf("evenement en rapport avec clik de la souris\n");
                                        widget = ei_widget_pick(&event->param.mouse.where);
                                        widget_prec = widget;
                                } else if  (event->type == ei_ev_mouse_buttonup) {
                                        printf("evenement en rapport avec clik de la souris\n");
                                        widget = ei_widget_pick(&event->param.mouse.where);
                                } else if (event->type == ei_ev_mouse_move && mvt_souris) {
                                        printf("evenement souris qui bouge\n");
                                        widget = widget_prec;
                                } 
                                // A GERER: si l'évenement est au clavier:
                                // ............

                                // on appelle le traitant si il s'applique au widget correspondant à l'endroit de l'evenement
                                bool traitant_pas_appele = true;
                                
                                while (traitant_pas_appele && cour != NULL) {

                                        if ((cour->tag == NULL && widget == cour->widget) || (cour->widget == NULL
                                                && (strncmp((char*) cour->tag, ei_widgetclass_stringname(widget->wclass->name), 8) == 0))) {
                                                // on appelle le traitant:
                                                
                                                arreter_appeler_callback = cour->callback(widget, event, cour->user_param);
                                                traitant_pas_appele = false;

                                        } else {
                                                cour = cour->suiv;
                                        }
                                }
                                if (cour == NULL) {
                                        arreter_appeler_callback = EI_TRUE;
                                }
                        } else {
                                arreter_appeler_callback = EI_TRUE;
                        }
                }
               
                if ((!sort_de_relacher_click) && (rects_a_changer != NULL)) {
/*
			printf("on redessine\n");
*/
                        // on redessine les différentes zones nécessitant un 
                        // rafraîchissement si il y en a:

                        // on bloque la surface de l'écran et celle de picking:
                        hw_surface_lock(surface_racine);
                        hw_surface_lock(surface_offscreen);
                        // on redessine
                        dessin_environnement();

                        // on débloque les surfaces:
                        hw_surface_unlock(surface_racine);
                        hw_surface_unlock(surface_offscreen);
/*
			printf("on a debloque les surfaces\n");
*/
                        // on copie les pixels sur l'écran si il y a des changements:
                        hw_surface_update_rects(surface_racine,rects_a_changer);
                }

                free(event);
                vider_rects(rects_a_changer);
                

        }
        //printf("fin app_run\n");
        getchar();
}

void ei_app_invalidate_rect(ei_rect_t* rect)
{
        ei_linked_rect_t *cour = rects_a_changer;
        while (cour != NULL) {
                printf( "%p\n", cour->next);
                cour = cour->next;
        }
        ei_linked_rect_t *nouveau_rect = calloc(1, sizeof (ei_linked_rect_t));
        if (nouveau_rect == NULL) {
                printf("nouveau_rect = null\n");
                exit(EXIT_FAILURE);
        }
        nouveau_rect->rect = *rect;
        nouveau_rect->next = rects_a_changer;
        rects_a_changer = nouveau_rect;
        
       
}

void ei_app_quit_request()
{
        sortie_boucle = true;
}

ei_widget_t* ei_app_root_widget()
{
        return (ei_widget_t*) racine;
}

ei_surface_t ei_app_root_surface()
{
        return surface_racine;
}
