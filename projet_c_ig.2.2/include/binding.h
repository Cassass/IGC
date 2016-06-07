#ifndef BINDING_H
#define BINDING_H

#include "ei_event.h"
#include "ei_types.h"
#include "ei_widget.h"

#include <stdbool.h>

bool mvt_souris, mvt_clavier,etait_sur_bouton,sort_de_relacher_click;
ei_widget_t *widget_prec;

struct ei_surface_t *surface_offscreen;

struct info_event {
	bool supprime_moi;
	ei_widget_t* widget;
	ei_tag_t tag;
	ei_callback_t callback;
	void* user_param;
	struct info_event* suiv;
};

struct info_event* binding[8];

void mise_a_jour_binding (struct info_event *binding[8]);

bool position_souris_sur_bouton (ei_point_t *where, struct ei_widget_t *widget);

ei_bool_t bouton_a_enfoncer (ei_widget_t *widget, struct ei_event_t *event, void *user_param);

ei_bool_t bouton_a_relacher (ei_widget_t *widget, struct ei_event_t *event, void *user_param);     

ei_bool_t deplacement_souris_bouton_appuye_sortant_bouton (ei_widget_t *widget, struct ei_event_t *event, void *user_param);

ei_bool_t deplacement_souris_bouton_appuye_entrant_bouton (ei_widget_t *widget, struct ei_event_t *event, void *user_param);

ei_bool_t relacher_click (ei_widget_t *widget, struct ei_event_t *event, void *user_param);

void vider_rects (ei_linked_rect_t * rects);



#endif
