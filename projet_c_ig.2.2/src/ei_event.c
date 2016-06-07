#include "ei_event.h"
#include "ei_types.h"
#include "ei_widget.h"
#include "binding.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>



void		ei_bind			(ei_eventtype_t		eventtype,
					 ei_widget_t*		widget,
					 ei_tag_t		tag,
					 ei_callback_t		callback,
					 void*			user_param)
{
        struct info_event *new = calloc (1, sizeof(struct info_event));
        if (new == NULL) {
                printf("new = null\n");
                exit (EXIT_FAILURE);
        }
        new->supprime_moi = false;
        new->widget = widget;
        new->tag = tag;
        new->callback = callback;
        new->user_param = user_param;
        
/*
        new->suiv = binding[eventtype];
        binding[eventtype] = new;
*/
        new->suiv = NULL;
        
        
        if (binding[eventtype] == NULL ){
                binding[eventtype] = new;                        
        } else if (binding[eventtype]->suiv == NULL) {
                binding[eventtype]->suiv = new;
        } else {
                struct info_event * last = binding[eventtype];
                while (last->suiv != NULL) {
                        last = last->suiv;
                }
                last->suiv =new;
        }
        
        
}

void		ei_unbind		(ei_eventtype_t		eventtype,
					 ei_widget_t*		widget,
					 ei_tag_t		tag,
					 ei_callback_t		callback,
					 void*			user_param)
{
        // recherche du bon truc à supprimer
        bool trouve = false;
        struct info_event *cour = binding[eventtype];
        while (!trouve && cour != NULL) {
                if (widget == cour->widget && tag == cour->tag 
                        && callback == cour->callback 
                        && user_param == cour->user_param) {
                        trouve = true;
                } else {
                        cour = cour->suiv;
                }
        }
        if (cour != NULL) {
                cour->supprime_moi = true;
        } else {
                printf("rien ne correspond\n");
        }
        
}
