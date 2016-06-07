#include "ei_placer.h"
#include "ei_geometrymanager.h"
#include "ei_types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

void runfunc_placer(ei_widget_t *widget)
{

        struct ei_placer_t *geom_new = (struct ei_placer_t*) widget->geom_params;


        widget->screen_location.top_left.y = geom_new->absolute_top_left_corner.y
                + floor(geom_new->relative_top_left_corner.y * widget->parent->screen_location.size.height)
                + widget->parent->screen_location.top_left.y;
        if (widget->screen_location.top_left.y < widget->parent->screen_location.top_left.y) {
                widget->screen_location.top_left.y = widget->parent->screen_location.top_left.y;
        } else if (widget->screen_location.top_left.y > widget->parent->screen_location.top_left.y + widget->parent->screen_location.size.height) {
                widget->screen_location.top_left.y = widget->parent->screen_location.top_left.y + widget->parent->screen_location.size.height;
        }



        widget->screen_location.top_left.x = geom_new->absolute_top_left_corner.x
                + floor(geom_new->relative_top_left_corner.x * widget->parent->screen_location.size.width)
                + widget->parent->screen_location.top_left.x;
        if (widget->screen_location.top_left.x < widget->parent->screen_location.top_left.x) {
                widget->screen_location.top_left.x = widget->parent->screen_location.top_left.x;
        } else if (widget->screen_location.top_left.x > widget->parent->screen_location.top_left.x + widget->parent->screen_location.size.width) {
                widget->screen_location.top_left.x = widget->parent->screen_location.top_left.x + widget->parent->screen_location.size.width;
        }



        widget->screen_location.size.width = geom_new->absolute_size.width
                + floor(geom_new->relative_size.width * widget->parent->requested_size.height);
        if (widget->screen_location.size.width > widget->parent->screen_location.size.width) {
                widget->screen_location.size.width = widget->parent->screen_location.size.width;
        }


        widget->screen_location.size.height = geom_new->absolute_size.height
                + floor(geom_new->relative_size.height * widget->parent->requested_size.height);
        if (widget->screen_location.size.height > widget->parent->screen_location.size.height) {
                widget->screen_location.size.height = widget->parent->screen_location.size.height;
        }

/*
        if ((widget->children_head != NULL) && (strncmp(widget->children_head->geom_params->manager->name, "placer", 6) == 0)) {
                ei_widget_t *cour = widget->children_head;
                while (cour != NULL) {
                        cour->geom_params->manager->runfunc(cour);
                        cour = cour->next_sibling;
                }
        }
*/
}

void releasefunc_placer(ei_widget_t *widget)
{
        ei_geometrymanager_unmap(widget);
        if ((widget->children_head->geom_params != NULL) &&
                (widget->children_head->geom_params->manager != NULL) &&
                (widget->children_head->geom_params->manager->releasefunc != NULL) &&
                (strncmp((char*) widget->children_head->geom_params->manager->name, "placeur", 7) == 0)) {

                releasefunc_placer(widget->children_head);
        }
}
