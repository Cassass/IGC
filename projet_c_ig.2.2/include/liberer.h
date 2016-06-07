#ifndef LIBERER_H
#define LIBERER_H

#include "ei_widget.h"

void supprimer_enfants (struct ei_widget_t *widget);

void detruire_widget (struct ei_widget_t *widget);

void app_free_recursive (struct ei_widget_t *widget);
        


#endif
