#ifndef EI_PLACER_H
#define EI_PLACER_H

#include "ei_geometrymanager.h"      

struct ei_geometrymanager_t *manager_placeur;


typedef struct {
	float x; 
	float y;
} ei_point_flottant_t;



typedef struct {
	float width;
	float height;
} ei_size_flottant_t;


typedef struct ei_placer_t {
        ei_geometrymanager_t manager;

        ei_point_flottant_t relative_top_left_corner;
        ei_size_flottant_t relative_size;

        ei_point_t absolute_top_left_corner;
        ei_size_t absolute_size;

} ei_placer_t;

void runfunc_placer(ei_widget_t *widget);

void releasefunc_placer(ei_widget_t *widget);

#endif
