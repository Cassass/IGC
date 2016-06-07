#include "liberer.h"
#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_widget_frame.h"
#include "ei_geometrymanager.h"

#include <stdio.h>
#include <stdlib.h>

void supprimer_enfants (struct ei_widget_t *widget)
{
       // printf("debut supprimer_enfants\n");
        struct ei_widget_t *cour, *suiv;
        cour = widget->children_head;
        while (cour != widget->children_tail) {
                suiv = cour->next_sibling;
                struct ei_widget_t *tmp = cour;
                app_free_recursive (tmp);
                cour = suiv;                
        }
        app_free_recursive (cour);
        //printf("fin supprimer_enfants\n");
}

void detruire_widget (struct ei_widget_t *widget) 
{
       // printf("debut detruire_widget\n");
        free(widget->pick_color);
        ei_geometrymanager_unmap (widget);
        free(widget->content_rect);
        widget->wclass->releasefunc(widget);
        ei_widget_destroy (widget);
      //  printf("fin detruire_widget\n");
}

void app_free_recursive (struct ei_widget_t *widget)
{
      //  printf("début app_free_récursive\n");
        if (widget->children_head == NULL) {
                detruire_widget (widget);
        } else {
                supprimer_enfants (widget);
                detruire_widget (widget);
        }
      //  printf("fin app_free_recursive\n");
}
