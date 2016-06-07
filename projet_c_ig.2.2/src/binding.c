#include "binding.h"
#include "ei_event.h"
#include "ei_widget.h"
#include "ei_widget_button.h"
#include "ei_types.h"
#include "ei_widget_button.h"
#include "ei_application.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


void vider_rects (ei_linked_rect_t * rects_a_changer)
{
        ei_linked_rect_t *cour = rects_a_changer;
	rects_a_changer = NULL;
	while (cour != NULL){
		ei_linked_rect_t *tmp =cour;
		cour = cour->next;
		free(tmp);	
	}	
}

void mise_a_jour_binding (struct info_event *binding[8])
{
        if (binding == NULL) {
                printf("binding == NULL");
                exit(EXIT_FAILURE);
        }
        for (uint8_t i = 0; i <= 7; i++) {

                //on supprime tous les binding en tete qui doivent l'etre
                while ((binding[i] != NULL) && (binding[i]->supprime_moi)) {
                        struct info_event *tmp = binding[i];
                        binding[i] = binding[i]->suiv;
                        free(tmp);
                }
                // a ce stade, la tete des binding n'est pas a supprimer,
                // on initialise les pointeurs prec et suiv si ils ont a l'etre
                // (i.e si il y a au moins 2 elmts dans la liste de binding n°i)
                if ((binding[i] != NULL) && (binding[i]->suiv != NULL)) {
                        struct info_event *prec = binding[i];
                        struct info_event *cour = prec -> suiv;
                        // on parcourt la liste de binding
                        while (cour != NULL) {
                                if (cour->supprime_moi) {
                                        //si le champs "supprime moi" du binding courant est a true,
                                        //on le supprime de la liste d'elmts.
                                        struct info_event *tmp = cour;
                                        cour = cour->suiv;
                                        prec->suiv = cour;
                                        free(tmp);
                                } else {
                                        // sinon on passe aux elmts suivants. 
                                        prec = cour;
                                        cour = cour->suiv;
                                }
                        }
                }
        }
}

bool position_souris_sur_bouton (ei_point_t *where, struct ei_widget_t *widget)
{
/*
        printf("début position souris\n");
*/
        if ((where->x >= widget->screen_location.top_left.x) &&
                (where->x <= widget->screen_location.top_left.x + widget->screen_location.size.width) &&
                (where->y >= widget->screen_location.top_left.y) &&
                (where->y <= widget->screen_location.top_left.y + widget->screen_location.size.height)) {
/*
                printf("fin true pos souris\n");
*/
                return true;
        } else {
/*
                printf("fin false pos souris\n");
*/
                return false;
        }
}

// pour enfoncer un bouton aprés un clik de la souris
ei_bool_t bouton_a_enfoncer (ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
/*
         printf("debut bouton a enfoncer\n");
*/
        
        
        sort_de_relacher_click=false;
        etait_sur_bouton = true;
	///!!!!\\\ a reflechir: le nombre de clik de la souris pressé a t il une importance???
        
       
	// on s'occupe d'enfoncer le bouton:
	struct ei_button_t *button = (struct ei_button_t *)widget;
	button->relief = ei_relief_sunken;
        
        // on appelle ei_app_invalidate_rect
        ei_app_invalidate_rect(&widget->screen_location);
                
	// on lie les évènements 
	// relâcher le bouton si la souris est relachée
	ei_bind (ei_ev_mouse_buttonup, NULL, "button", bouton_a_relacher , NULL);
	// si on sort du bouton, en restant appuyé avec la souris, il faut le relacher
	ei_bind (ei_ev_mouse_move, widget, NULL, deplacement_souris_bouton_appuye_sortant_bouton, NULL);
        mvt_souris = true;
/*
        printf("fin bouton a enfoncer\n");
*/
        
	return EI_TRUE; //  AVOIR SI CEST VRMT TRUE!!!
}


ei_bool_t bouton_a_relacher (ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
/*
        printf("début bouton a relacher\n");
*/
        // on appelle ei_app_invalidate_rect
        ei_app_invalidate_rect(&widget->screen_location);
        etait_sur_bouton = false;
        
	struct ei_button_t *button = (struct ei_button_t *)widget;
	// on change l'apparence du bouton qui n'est plus enfoncé:
	button->relief = ei_relief_raised;
        
        
	
	// on défait les liens qu'il faut:
	ei_unbind(ei_ev_mouse_buttonup, NULL, "button", bouton_a_relacher, NULL);
        ei_unbind(ei_ev_mouse_move, widget, NULL, deplacement_souris_bouton_appuye_sortant_bouton, NULL);
/*
        printf("aprés les unbind\n");
*/

        mvt_souris = false;
        ///////!!!!!!!!!\\\\
		// a gérer: faire si le bouton est de type pr fermer ou pr aggrandir faire des bind correspondnt aux actions que l'on veut!!!!!!!!
/*
        printf("fin bouton a relacher true\n");
*/
        return EI_FALSE;

}

ei_bool_t deplacement_souris_bouton_appuye_sortant_bouton (ei_widget_t *widget, struct ei_event_t *event, void *user_param) 
{
        ei_unbind (ei_ev_mouse_buttonup, NULL, "button", bouton_a_relacher , NULL);
        ei_unbind (ei_ev_mouse_move, widget, NULL, deplacement_souris_bouton_appuye_sortant_bouton, NULL);
        ei_bind (ei_ev_mouse_buttonup, NULL, "frame", relacher_click, NULL);
        ei_bind (ei_ev_mouse_move, widget, NULL, deplacement_souris_bouton_appuye_entrant_bouton, NULL);
        
        if (widget == ei_widget_pick (&event->param.mouse.where)) {
/*
                printf("on est tjs sur le bouton ANORMAL\n");
*/
        } else {
                etait_sur_bouton = false;
/*
                printf("on est sorti du bouton\n");
*/
                struct ei_button_t *button = (struct ei_button_t *) widget;
                // on change l'apparence du bouton qui n'est plus enfoncé:
                button->relief = ei_relief_raised;

                // on appelle ei_app_invalidate_rect
                ei_app_invalidate_rect(&widget->screen_location);
        }
        return EI_TRUE;
}

ei_bool_t deplacement_souris_bouton_appuye_entrant_bouton (ei_widget_t *widget, struct ei_event_t *event, void *user_param) 
{
        ei_unbind (ei_ev_mouse_move, widget, NULL, deplacement_souris_bouton_appuye_entrant_bouton, NULL);
        ei_unbind (ei_ev_mouse_buttonup, NULL, "button", relacher_click, NULL);
        ei_bind (ei_ev_mouse_buttonup, NULL, "button", bouton_a_relacher , NULL);
        ei_bind (ei_ev_mouse_move, widget, NULL, deplacement_souris_bouton_appuye_sortant_bouton, NULL);
        
        if ( widget != ei_widget_pick (&event->param.mouse.where) ) {
/*
                printf("on est plus sur bouton ANORMAL\n");
*/
        } else {
/*
                printf ("on est sur le bouton\n");
*/
                etait_sur_bouton = true;
                struct ei_button_t *button = (struct ei_button_t *) widget;
                // on change l'apparence du bouton qui n'est plus enfoncé:
                button->relief = ei_relief_sunken;

                // on appelle ei_app_invalidate_rect
                ei_app_invalidate_rect(&widget->screen_location);
        }
        return EI_TRUE;
}

ei_bool_t relacher_click (ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
        sort_de_relacher_click = true;
        mvt_souris = false;
        ei_unbind (ei_ev_mouse_buttonup, NULL, "frame", relacher_click, NULL);
        ei_unbind (ei_ev_mouse_move, widget, NULL, deplacement_souris_bouton_appuye_entrant_bouton, NULL);
        return EI_TRUE;
}


////////////////////////GESTION CLAVIER //////////////////


ei_bool_t touche_enfoncee (ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
        
        
	/* if (key->key_sym == "escape" ) { */
	/* 	ei_bind (ei_ev_keyup, NULL, "top_level", tout_fermer , NULL); */
	/* 	return EI_FALSE ; */
	/* } else if (est_modifier(key->key_sym)){ // il faut probablement utiliser ei_has_modifier (ei_event.h) */
	/* 	ei_bind (ei_ev_keydown,NULL,"top_level", touche_enfoncee_apres_modifier,NULL); */
	/* 	key->modifier_mask=key->sym; */
	/* 	ei_bind (ei_ev_keyup,NULL, "top_level", stop_modifier, NULL); */
	/* 	return EI_FALSE */
	/* } else { */
	/* 	ei_bind (ei_ev_key_up,NULL, "top_level", touche_relevee, NULL); */
	/* 	return EI_TRUE ;  */
	/* }      */  
}

ei_bool_t touche_relevee (ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
	ei_unbind (ei_ev_keyup,NULL, "top_level", touche_relevee, NULL);
	return EI_TRUE;
}

ei_bool_t tout_fermer (ei_widget_t *widget,struct ei_event_t *event, void *user_param)
{
}
