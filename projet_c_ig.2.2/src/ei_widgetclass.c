#include "ei_widgetclass.h"
#include "ei_widget_frame.h"
#include "ei_widget_button.h"
#include "ei_widget_toplevel.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct ei_widgetclass_t *classes;

void ei_widgetclass_register(ei_widgetclass_t* widgetclass)
{
        // doit gérer widgetclass->next
        if (classes == NULL) {
                classes = widgetclass;
                widgetclass->next = NULL;
        } else {
                widgetclass->next = classes;
                classes = widgetclass;
        }
}

ei_widgetclass_t* ei_widgetclass_from_name(ei_widgetclass_name_t name)
{
        if (strncmp(ei_widgetclass_stringname(name), "frame", 5) == 0) {
                return frame_class;
        } else if (strncmp(ei_widgetclass_stringname(name), "button", 6) == 0) {
                return button_class;
        } else if (strncmp(ei_widgetclass_stringname(name), "toplevel", 8) == 0) {
                return toplevel_class;
        } else {
		return NULL;
	} 

}

void ei_frame_register_class()
{

        frame_class = calloc(1, sizeof (struct ei_widgetclass_t));
        if (frame_class == NULL) {
                printf("frame_class= NULL\n");
                exit(EXIT_FAILURE);
        }
        strcpy(frame_class->name, "frame");
        frame_class->allocfunc = alloc_frame;
        frame_class->releasefunc = release_frame;
        frame_class->drawfunc = draw_frame;
        frame_class->setdefaultsfunc = setdefaults_frame;
        frame_class->geomnotifyfunc = geom_frame;

}

void ei_button_register_class()
{
        button_class = calloc(1, sizeof (struct ei_widgetclass_t));
        if (button_class == NULL) {
                printf("button_class= NULL\n");
                exit(EXIT_FAILURE);
        }
        strcpy(button_class->name, "button");
        button_class->allocfunc = alloc_button;
        button_class->releasefunc = release_button;
        button_class->drawfunc = draw_button;
        button_class->setdefaultsfunc = setdefaults_button;
        button_class->geomnotifyfunc = geom_button;
}

void ei_toplevel_register_class()
{
        toplevel_class = calloc (1, sizeof(struct ei_widgetclass_t));
        if (toplevel_class == NULL) {
                printf("toplevel_class = NULL\n");
                exit(EXIT_FAILURE);
        }
        strcpy(toplevel_class->name, "toplevel");
        toplevel_class->allocfunc = alloc_toplevel;
        toplevel_class->releasefunc = release_toplevel;
        toplevel_class->drawfunc = draw_toplevel;
        toplevel_class->setdefaultsfunc = setdefaults_toplevel;
        toplevel_class->geomnotifyfunc = geom_toplevel;
}
