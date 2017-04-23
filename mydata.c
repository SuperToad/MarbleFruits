
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "curve.h"
#include "mydata.h"


Mydata *get_mydata (gpointer data){
	if (data == NULL) {
		fprintf (stderr, "get_mydata: NULL data\n"); 
		return NULL;
	}
	if (((Mydata *)data)->magic != MYDATA_MAGIC) {
		fprintf (stderr, "get_mydata: bad magic number\n"); 
		return NULL;
	}
	return data;
}

void init_data(Mydata *data){
	data->title = "Marbles Fruits 2 : Fruits' Revenge";
	data->win_height = 600;
	data->win_width = 800;
	data->magic = MYDATA_MAGIC;
	data->current_folder = NULL;
	data->pixbuf1 = NULL;
	data->pixbuf2 = NULL;
	data->rotate_angle = 0.0;
	data->vbox1 = NULL;
	data->scale1 = NULL;
	data->scale_value = 1.0;
	data->last_x = 0;
	data->last_y = 0;	
	data->click_x = 0;
	data->click_y = 0;
	data->click_n = 0;
	data->count = 0;
	data->clip_image = FALSE;
	data->show_edit = FALSE;
	data->scroll = NULL;
	data->menu_bar = NULL;
	data->frame1 = NULL;
	data->hbox1 = NULL;
	data->pixbuf_canon = NULL;
	data->pixbuf_canon2 = NULL;
	data->edit_mode = EDIT_ADD_CURVE;
	data->bsp_mode = BSP_OPEN;
	
	data->game = malloc (sizeof (Game));
	
	data->logo = cairo_image_surface_create_from_png ("./images/logo.png");
	
	data->canon = cairo_image_surface_create_from_png ("./images/dk.png");
	
	data->orange = cairo_image_surface_create_from_png ("./images/orange.png");
	data->pomme = cairo_image_surface_create_from_png ("./images/pomme.png");
	data->pasteque = cairo_image_surface_create_from_png ("./images/pasteque.png");
	
	init_curve_infos(&data->curve_infos);
}

void set_edit_mode (Mydata *data, int mode) {
	Mydata *my = get_mydata(data);
	if (mode > EDIT_NONE || mode < EDIT_LAST) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (my->edit_radios[mode-1]), TRUE);
	}
}

void canon_init (Game * game, int win_width, int win_height)
{
	game->canon.cx = win_width/2;
	game->canon.cy = win_height/2;
	game->canon.angle = 0.0;
	game->canon.reload = 0.0;
	game->canon.reload_time = 5;
	game->canon.ammo1 = rand()%3;
	game->canon.ammo2 = rand()%3;
	
	
	
}

void scan_levels (Game * game)
{
	FILE *file;
	do
	{
		char s[50];
		sprintf(s, "./levels/%d.txt", game->level_list.level_count);
		
		file = fopen (s, "r");
		
		game->level_list.level_count++;
	}
	while (file != NULL);
	
	game->level_list.level_count--;
}

void game_init (Game * game, int win_width, int win_height)
{
	static const Game gameInit;
	*game = gameInit;

	canon_init (game, win_width, win_height);
	
	game->g_rand = g_rand_new_with_seed (10); // change seed
	game->state = GS_HELLO;
	
	game->shot_list.shot_count = 0;
	game->score = 0;
	game->current_level = 0;
	game->level_list.level_count = 0;
	game->diameter = 30;
	game->total_marbles = 10;
	game->initial_speed = 10.0;
	game->speed = game->initial_speed;
	
	scan_levels (game);
	printf ("Found %d levels\n", game->level_list.level_count);
	
}
