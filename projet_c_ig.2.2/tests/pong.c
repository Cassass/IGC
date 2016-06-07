/*
 *  pong.c
 *  ig
 *
 *  Created by François Bérard on 06.06.14.
 *  Copyright 2011 Ensimag. All rights reserved.
 *
 */


#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_widget.h"
#include "ei_utils.h"
#include "ei_event.h"
#include "ei_geometrymanager.h"


static char*			g_ball_filename			= "misc/ball.png";
static ei_point_t		g_toplevel_position		= {30, 10};
static ei_size_t		g_toplevel_size			= { 800, 600 };
static ei_color_t		g_toplevel_bg_color		= { 0xff, 0xff, 0xff, 0xff };
static char			g_toplevel_title[]		= "Pong!";

static ei_widget_t*		g_ball				= NULL;
static ei_surface_t		g_ball_image			= NULL;
static ei_size_t		g_ball_size			= { -1, -1 };
static ei_point_t		g_ball_midsize			= { -1, -1 };
static ei_point_t		g_ball_position			= { -1, -1 };
static ei_point_t		g_ball_speed			= { 12, -6 };
static ei_color_t		g_ball_color			= { 0x00, 0xff, 0x00, 0x00 };

static ei_widget_t*		g_rackets[2]			= { NULL, NULL };
static ei_point_t		g_rackets_pos[2];
static ei_size_t		g_rackets_size			= { 20, 120 };
static ei_color_t		g_rackets_color			= { 0xff, 0x00, 0x00, 0xff };
static int 			g_racket_top_margin		= 10;

static int			g_anim_period_ms		= 50;



// handle_keydown --

ei_bool_t handle_keydown(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
	if (event->param.key.key_sym == SDLK_ESCAPE) {
		ei_app_quit_request();
		return EI_TRUE;
	}

	return EI_FALSE;
}



// handle_anim --

ei_bool_t handle_anim(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
	ei_point_t		min, max;

	g_ball_position				= ei_point_add(g_ball_position, g_ball_speed);
	
	max					= ei_point_add(g_ball_position, g_ball_midsize);
	min					= ei_point_sub(g_ball_position, g_ball_midsize);
	
	
	if (min.x < 0) {
		g_ball_position.x		+= 2 * -min.x;
		g_ball_speed.x			= -g_ball_speed.x;
	}
	if (max.x > g_toplevel_size.width) {
		g_ball_position.x		-= 2 * (max.x - g_toplevel_size.width);
		g_ball_speed.x			= -g_ball_speed.x;
	}
	if (min.y < 0) {
		g_ball_position.y		+= 2 * -min.y;
		g_ball_speed.y			= -g_ball_speed.y;
	}
	if (max.y > g_toplevel_size.height) {
		g_ball_position.y		-= 2 * (max.y - g_toplevel_size.height);
		g_ball_speed.y			= -g_ball_speed.y;
	}
	
	ei_place(g_ball, NULL, &(g_ball_position.x), &(g_ball_position.y), NULL, NULL, NULL, NULL, NULL, NULL);

	hw_event_schedule_app(g_anim_period_ms, NULL);
	return EI_TRUE;
}



// create_pong_window --

void create_pong_window()
{
	ei_widget_t*		root			= ei_app_root_widget();

	ei_bool_t		use_toplevel		= EI_FALSE;
	ei_widget_t*		toplevel;
	ei_axis_set_t		top_resizable		= ei_axis_none;
	ei_bool_t		top_closable		= EI_FALSE;
	char*			top_title		= g_toplevel_title;
	ei_anchor_t		top_anchor		= ei_anc_northwest;

	int			ball_bd			= 0;
	ei_anchor_t		ball_anchor		= ei_anc_center;
	
	ei_surface_t		tmp_surface		= NULL;
	ei_size_t		tmp_surface_size	= ei_size(10, 10);

	int			i;
	int			racket_bd		= 4;
	ei_relief_t		racket_relief		= ei_relief_raised;
	ei_anchor_t		racket_anchor[2]	= { ei_anc_northwest, ei_anc_northeast };

	// Toplevel

	toplevel			= ei_widget_create("toplevel", ei_app_root_widget());
	ei_toplevel_configure(toplevel, &g_toplevel_size, &g_toplevel_bg_color, NULL, &top_title, &top_closable, &top_resizable, NULL);

	if (use_toplevel) {
		root = toplevel;
		ei_place(toplevel, &top_anchor, &(g_toplevel_position.x), &(g_toplevel_position.y), NULL, NULL, NULL, NULL, NULL, NULL);

	} else
		g_toplevel_size		= hw_surface_get_size(ei_app_root_surface());

	// Ball frame
	// 	Create a temporary surface that has the same channel indices as the root surface, except it as alpha.

	tmp_surface			= hw_surface_create(ei_app_root_surface(), &tmp_surface_size, EI_TRUE);
	g_ball_image			= hw_image_load(g_ball_filename, tmp_surface);
	hw_surface_free(tmp_surface);
	if (g_ball_image == NULL) {
		printf("Could not load ball image \"%s\".\n", g_ball_filename);
		exit(1);
	}
	g_ball_size			= hw_surface_get_size(g_ball_image);
	g_ball_midsize			= ei_point(g_ball_size.width / 2, g_ball_size.height / 2);

	g_ball				= ei_widget_create("frame", root);
	ei_frame_configure(g_ball, &g_ball_size, &g_ball_color, &ball_bd, NULL, NULL, NULL, NULL, NULL, &g_ball_image, NULL, NULL);

	g_ball_position			= ei_point(g_toplevel_size.width / 2, g_toplevel_size.height / 2);
	ei_place(g_ball, &ball_anchor, &g_ball_position.x, &g_ball_position.y, &g_ball_size.width, &g_ball_size.height, NULL, NULL, NULL, NULL);


	// Rackets

	for (i = 0; i < 2; i++) {
		g_rackets[i]		= ei_widget_create("frame", root);
		ei_frame_configure(g_rackets[i], &g_rackets_size, &g_rackets_color, &racket_bd, &racket_relief, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

		g_rackets_pos[i]	= ei_point(g_toplevel_size.width * i + g_racket_top_margin, 10);
		g_racket_top_margin	= -g_racket_top_margin;
		ei_place(g_rackets[i], &(racket_anchor[i]), &(g_rackets_pos[i].x), &(g_rackets_pos[i].y), NULL, NULL, NULL, NULL, NULL, NULL);
	}
}



// ei_main --

int ei_main(int argc, char* argv[])
{
	ei_size_t	root_window_size		= { 1024, 768 };
	ei_color_t	root_bgcol			= { 0x52, 0x7f, 0xb4, 0xff };
	ei_bool_t	fullscreen			= EI_FALSE;

	ei_app_create(&root_window_size, fullscreen);
	ei_frame_configure(ei_app_root_widget(), NULL, &root_bgcol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	create_pong_window();

	ei_bind(ei_ev_keydown,	NULL, "all", handle_keydown, 	NULL);
	ei_bind(ei_ev_app,	NULL, "all", handle_anim,	NULL);

	hw_event_schedule_app(g_anim_period_ms, NULL);

	ei_app_run();
	
	ei_app_free();

	return (EXIT_SUCCESS);
}
