#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "font.h"
#include "util.h"
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "drawings.h"


void apply_image_transforms (Mydata *data) {	
	Mydata *my = get_mydata(data);	
	g_clear_object(&my->pixbuf2);
	if (my->pixbuf1 == NULL) return;
	printf ("pixbuf1 : %d x %d, angle %f\n", gdk_pixbuf_get_width (my->pixbuf1), gdk_pixbuf_get_height (my->pixbuf1), my->rotate_angle);
	GdkPixbuf *tmp = gdk_pixbuf_rotate_simple (my->pixbuf1, my->rotate_angle);
	int width = gdk_pixbuf_get_width (tmp);
	int height = gdk_pixbuf_get_height (tmp);	
	printf ("tmp : %d x %d\n", width, height);
	
	my->pixbuf2 = gdk_pixbuf_scale_simple (tmp, width * my->scale_value, height * my->scale_value, GDK_INTERP_BILINEAR);
	printf ("pixbuf2 : %d x %d\n", gdk_pixbuf_get_width (my->pixbuf2), gdk_pixbuf_get_height (my->pixbuf2));
	
	g_object_unref (tmp);
}


void update_area1_with_transforms (Mydata *data) {
	Mydata *my = get_mydata(data);
	apply_image_transforms (my);
	if (my->pixbuf2 != NULL) {
		gtk_widget_set_size_request (my->area1, gdk_pixbuf_get_width (my->pixbuf2), gdk_pixbuf_get_height (my->pixbuf2));	
		refresh_area (my->area1);	
	}
}

void draw_control_polygons (cairo_t *cr, Curve_infos *ci) {
	Curve *curve;
	for (int i = 0; i < ci->curve_list.curve_count; ++i)	{
		curve = &ci->curve_list.curves[i];
		cairo_move_to (cr, curve->controls[0].x, curve->controls[0].y);
		for (int j = 0; j < curve->control_count; ++j) {
	        cairo_set_line_width (cr, 3);
	        if (i == ci->current_curve)
	        	cairo_set_source_rgb (cr, 1.0, 1.0, 0);
	        else
	        	cairo_set_source_rgb (cr, 0.6, 0.6, 0.6);
	        cairo_line_to (cr, curve->controls[j].x, curve->controls[j].y);		
		}
		cairo_stroke (cr);
	}

	for (int i = 0; i < ci->curve_list.curve_count; ++i)	{
		curve = &ci->curve_list.curves[i];
		for (int j = 0; j < curve->control_count; ++j) {
	        cairo_set_line_width (cr, 3);
	        if (i == ci->current_curve && j == ci->current_control)
	        	cairo_set_source_rgb (cr, 1.0, 0, 0);
	        else
	        	cairo_set_source_rgb (cr, 0, 0, 1.0);
	        cairo_rectangle (cr, curve->controls[j].x - 3.0, 
	        				 curve->controls[j].y - 3.0, 6.0, 6.0);
	        cairo_stroke (cr);			
		}

	}
}

/* Game */

void draw_canon (cairo_t *cr, Mydata *my)
{
	if (my->canon != NULL)
	{
		cairo_save (cr);
		
		int ima_w = cairo_image_surface_get_width (my->canon),
		ima_h = cairo_image_surface_get_height (my->canon);
		
		cairo_surface_t *ammo1, *ammo2;
		ammo1 = my->orange;
		ammo2 = my->orange;
		if (my->game->canon.ammo1 == ORANGE)
			ammo1 = my->orange;
		if (my->game->canon.ammo1 == POMME)
			ammo1 = my->pomme;
		if (my->game->canon.ammo1 == PASTEQUE)
			ammo1 = my->pasteque;
		if (my->game->canon.ammo2 == ORANGE)
			ammo2 = my->orange;
		if (my->game->canon.ammo2 == POMME)
			ammo2 = my->pomme;
		if (my->game->canon.ammo2 == PASTEQUE)
			ammo2 = my->pasteque;
			
		int ima_wa1 = cairo_image_surface_get_width (ammo1),
		ima_ha1 = cairo_image_surface_get_height (ammo1);
		
		int ima_wa2 = cairo_image_surface_get_width (ammo2),
		ima_ha2 = cairo_image_surface_get_height (ammo2);
		
		int cx = my->game->canon.cx - ima_w/2;
		int cy = my->game->canon.cy - ima_h/2;
		
		cairo_identity_matrix (cr);
		cairo_translate (cr, cx + ima_w/2, cy + ima_h/2);
		cairo_rotate (cr, my->game->canon.angle);
		cairo_scale (cr, 0.4, 0.4);
		cairo_translate (cr, -cx - ima_w/2, -cy - ima_h/2);
		
		cairo_set_source_surface (cr, my->canon, cx, cy);
		cairo_rectangle (cr, cx, cy, ima_w, ima_h);
		cairo_fill (cr);
		
		cairo_translate (cr, cx - 100 + ima_wa1/2, cy +60);
		cairo_scale (cr, 0.13, 0.13);
		cairo_translate (cr, -cx + 100 - ima_wa1/2, -cy -60);
		cairo_set_source_surface (cr, ammo1, cx, cy);
		cairo_rectangle (cr, cx, cy, ima_wa1, ima_ha1);
		cairo_fill (cr);
		
		cairo_translate (cr, cx - 1600 + ima_wa2/2, 0);
		cairo_set_source_surface (cr, ammo2, cx, cy);
		cairo_rectangle (cr, cx, cy, ima_wa2, ima_ha2);
		cairo_fill (cr);
		cairo_restore (cr);
	}

}

void draw_shots (cairo_t *cr, Mydata *my)
{
	int i;
	for (i = 0; i < my->game->shot_list.shot_count; i++)
	{
		cairo_save (cr);
		
		
		cairo_surface_t *fruit;
		fruit = my->orange;
		if (my->game->shot_list.shots[i].color == ORANGE)
			fruit = my->orange;
		if (my->game->shot_list.shots[i].color == POMME)
			fruit = my->pomme;
		if (my->game->shot_list.shots[i].color == PASTEQUE)
			fruit = my->pasteque;
		
		int ima_w = cairo_image_surface_get_width (fruit),
		ima_h = cairo_image_surface_get_height (fruit);
		
		int cx = my->game->shot_list.shots[i].x - ima_w/2;
		int cy = my->game->shot_list.shots[i].y - ima_h/2;
		
		cairo_identity_matrix (cr);
		cairo_translate (cr, cx + ima_w/2, cy + ima_h/2);
		cairo_scale (cr, 0.05, 0.05);
		cairo_translate (cr, -cx - ima_w/2, -cy - ima_h/2);
		
		cairo_set_source_surface (cr, fruit, cx, cy);
		cairo_rectangle (cr, cx, cy, ima_w, ima_h);
		cairo_fill (cr);
		
		cairo_restore (cr);
		
		
	}
}

void draw_train_tracks (cairo_t *cr, Mydata *my)
{
	int i, j, w;
	int width = 10;
	double color = 0.2;
	int count = my->game->track_list.track_count;
	for (i = 0; i < count; i++)
	{
		//Track t = my->game->track_list.tracks[i];
		for (w = width; w >= 0; w -= 4)
		{
			cairo_set_source_rgb (cr, color, color, color);
			cairo_set_line_width (cr, w);
			
			cairo_move_to (cr, my->game->track_list.tracks[i].sample_x[0], my->game->track_list.tracks[i].sample_y[0]);
			int track_count = my->game->track_list.tracks[i].sample_count;
			for (j = 0; j < track_count; j++)
				cairo_line_to (cr, my->game->track_list.tracks[i].sample_x[j], my->game->track_list.tracks[i].sample_y[j]);
			
			
			cairo_stroke (cr);
			cairo_arc (cr, my->game->track_list.tracks[i].sample_x[track_count - 1], my->game->track_list.tracks[i].sample_y[track_count - 1], w*3, .0, 2 * 3.14);
			cairo_fill (cr);
			color += 0.2;
		}
		color = 0.2;
		
	}
	
}


void draw_train_marbles (cairo_t *cr, Mydata *my)
{
	int i, j = 0;

	int count = my->game->track_list.track_count;
	int diameter = my->game->diameter;
	PangoLayout *layout = pango_cairo_create_layout (cr);
	int marble_count, first_visible;
	for (i = 0; i < count; i++)
	{
		marble_count = my->game->track_list.tracks[i].marble_count;
		first_visible = my->game->track_list.tracks[i].first_visible;
		cairo_surface_t *fruit;
		for (j = first_visible; j < marble_count; j++)
		{
			if (j >= 0)
			{	
			
				switch (my->game->track_list.tracks[i].marbles[j].color){
					case ORANGE:
						fruit = my->orange;
						break;
					case POMME:
						fruit = my->pomme;
						break;
					case PASTEQUE:
						fruit = my->pasteque;
						break;
					default:
						fruit = my->orange;
						break;
				}
				
				int ima_w = cairo_image_surface_get_width (fruit),
				ima_h = cairo_image_surface_get_height (fruit);
				
				int x = my->game->track_list.tracks[i].marbles[j].x - diameter/2;
				int y = my->game->track_list.tracks[i].marbles[j].y - diameter/2;
				
				cairo_identity_matrix (cr);
				cairo_translate (cr, x, y + diameter);
				cairo_scale (cr, 0.05, 0.05);
				cairo_translate (cr, -x, -y - diameter);
				
				//afficher marble en x y
				cairo_set_source_surface (cr, fruit, x, y);
				cairo_rectangle (cr, x, y, ima_w, ima_h);
				cairo_fill (cr);
				
				cairo_identity_matrix (cr);
				font_set_name (layout, "Sans 8");
				cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
				font_draw_text (cr, layout, FONT_TC, x + diameter/2, y - 10, "%d", j);
			}	
		}
	}
	
}


void draw_score (cairo_t *cr, Mydata *my)
{
	PangoLayout *layout = pango_cairo_create_layout (cr);
	font_set_name (layout, "Sans 12");
	cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
	font_draw_text (cr, layout, FONT_TC, 50, 0, "Niveau : %d\nScore : %d", my->game->current_level, my->game->score);
	
}

void check_end_of_game (Mydata *my)
{
	Game * game = my->game;
	int rayon = game->diameter/2;
	if (game->state == GS_PLAYING)
	{
		int i, win_flag = 1;
		int count = game->track_list.track_count;
		
		for (i = 0; i < count; i++)
		{
			if (game->track_list.tracks[i].marble_count > 0)
				win_flag = 0;
		}
		
		if (win_flag == 1)
		{
			printf ("VICTORY !!!\n");
			game->state = GS_WON;
			
			game->current_level++;
			if (game->current_level > game->level_list.level_count - 1)
			{
				game->current_level = 0;
				game->total_marbles *= 2;
				game->initial_speed *= 2;
			}
				
				
			char s[50];
			sprintf(s, "You won ! Go to the next level : %d/%d", game->current_level, game->level_list.level_count);
			set_status(my->status, s);
		}
		
		for (i = 0; i < count; i++)
		{
			int marble_count = game->track_list.tracks[i].marble_count;
			
			int dist_x = game->track_list.tracks[i].marbles[marble_count - 1].x - game->track_list.tracks[i].sample_x[game->track_list.tracks[i].sample_count - 1];
			int dist_y = game->track_list.tracks[i].marbles[marble_count - 1].y - game->track_list.tracks[i].sample_y[game->track_list.tracks[i].sample_count - 1];

			if ( dist_x*dist_x + dist_y*dist_y <= rayon*rayon)
			{
				printf ("t : %lf\n", game->track_list.tracks[i].marbles[marble_count - 1].t);
				printf ("DEFEAT !!!\n");
				game->state = GS_LOST;
				//game->current_level = 0;
				my->game->score = 0;
				set_status(my->status, "You lost! Retry your luck ");
				
				
			}

		}
	}
	
	
}

void draw_bg (cairo_t *cr, Mydata *my)
{
	//char *filename;
	//filename = "./images/BG_sunset.png";
	
	cairo_save (cr);
	
	cairo_surface_t *bg = cairo_image_surface_create_from_png ("./images/BG_sunset.png");
	
	int ima_w = cairo_image_surface_get_width (bg),
		ima_h = cairo_image_surface_get_height (bg);
		
	int cx = - ima_w/2;
	int cy = - ima_h/2;
	
	cairo_identity_matrix (cr);
	cairo_translate (cr, -cx - ima_w/2, -cy - ima_h/2);
	
	cairo_set_source_surface (cr, bg, cx, cy);
	cairo_rectangle (cr, cx, cy, ima_w, ima_h);
	cairo_fill (cr);
	
	
}

gboolean on_timeout1 (gpointer data)
{
	Mydata *my = data;
	my->count++;
	if (my->game->canon.reload > 0)
		my->game->canon.reload--;
	if (my->game->state == GS_PLAYING)
		progress_game_next_step (my->game, my->win_width, my->win_height);
	if (my->game->state == GS_LOST)
		move_trains_one_step (my->game);
	check_end_of_game (my);
	refresh_area (my->area1);
	return TRUE;
}

// CALLBACKS

gboolean on_area1_key_press (GtkWidget *area, GdkEvent *event, gpointer data){
	Mydata *my = get_mydata(data);
	GdkEventKey *evk = &event->key;
	printf ("%s: GDK_KEY_%s\n",	__func__, gdk_keyval_name(evk->keyval));
	switch (evk->keyval) {
		case GDK_KEY_q : gtk_widget_destroy(my->window); break;
		case GDK_KEY_a : set_edit_mode (my, EDIT_ADD_CURVE); break;
		case GDK_KEY_z : set_edit_mode (my, EDIT_MOVE_CURVE); break;
		case GDK_KEY_e : set_edit_mode (my, EDIT_REMOVE_CURVE); break;
		case GDK_KEY_r : set_edit_mode (my, EDIT_ADD_CONTROL); break;
		case GDK_KEY_t : set_edit_mode (my, EDIT_MOVE_CONTROL); break;
		case GDK_KEY_y : set_edit_mode (my, EDIT_REMOVE_CONTROL); break;
		case GDK_KEY_space : if (my->game->state == GS_PLAYING) swap_ammo (my->game); break;
		case GDK_KEY_p : my->game->state =(my->game->state == GS_PLAYING)?  GS_PAUSE : GS_PLAYING; break;
	}
	return TRUE;  // evenement traite
}


gboolean on_area1_key_release (GtkWidget *area, GdkEvent *event, gpointer data){
	GdkEventKey *evk = &event->key;
	printf ("%s: GDK_KEY_%s\n",	__func__, gdk_keyval_name(evk->keyval));
	return TRUE;  // evenement traite
}

gboolean on_area1_button_press (GtkWidget *area, GdkEvent *event, gpointer data){
    Mydata *my = get_mydata(data);	 
	int n;
	GdkEventButton *evb = &event->button;
	printf ("%s: %d %.1f %.1f\n", __func__, evb->button, evb->x, evb->y);
	my->click_y = evb->y;
	my->click_x = evb->x;
	my->click_n = evb->button;
	if (my->click_n == 1 && my->show_edit == TRUE) {
		switch (my->edit_mode) {
			case EDIT_ADD_CURVE : 
				n = add_curve (&my->curve_infos);
				if(n < 0) break;
				set_edit_mode (my, EDIT_ADD_CONTROL);
				add_control (&my->curve_infos, my->click_x, my->click_y);
				refresh_area (my->area1);
				break;
			case EDIT_MOVE_CURVE : 
				find_control (&my->curve_infos, my->click_x, my->click_y);				
				refresh_area (my->area1); 
				break;
			case EDIT_REMOVE_CURVE : 
				n = find_control (&my->curve_infos, my->click_x, my->click_y);
				if (n == 0) remove_curve (&my->curve_infos);
				refresh_area (my->area1); 
				break;
			case EDIT_ADD_CONTROL : 
				add_control (&my->curve_infos, my->click_x, my->click_y);
				refresh_area (my->area1); 
				break;
			case EDIT_MOVE_CONTROL : 
				find_control (&my->curve_infos, my->click_x, my->click_y);
				refresh_area (my->area1); 
				break;
			case EDIT_REMOVE_CONTROL : 
				n = find_control (&my->curve_infos, my->click_x, my->click_y);
				if (n == 0) remove_control (&my->curve_infos);			
				refresh_area (my->area1); 
				break;
			case EDIT_MOVE_CLIP : 
				find_control (&my->curve_infos, my->click_x, my->click_y);
				refresh_area(my->area1);
				break;
			case EDIT_RESET_CLIP : 
				find_control (&my->curve_infos, my->click_x, my->click_y);
				reset_shift (&my->curve_infos);
				break;
		}
	}

	if ((my->game->state == GS_PLAYING) && (evb->type ==  GDK_BUTTON_PRESS))
		shoot_ammo (my->game, my->click_x, my->click_y);

	refresh_area(my->area1);
	return TRUE;  // evenement traite
}

gboolean on_area1_button_release (GtkWidget *area, GdkEvent *event, gpointer data){
    Mydata *my = get_mydata(data);	 
	GdkEventButton *evb = &event->button;
	printf ("%s: %d %.1f %.1f\n", __func__, evb->button, evb->x, evb->y);
	my->click_n = 0;
	refresh_area(my->area1);
	return TRUE;  // evenement traite
}

gboolean on_area1_motion_notify (GtkWidget *area, GdkEvent *event, gpointer data){
    Mydata *my = get_mydata(data);	 
	GdkEventMotion *evm = &event->motion;	
	my->last_y = my->click_y;
	my->last_x = my->click_x ;
	my->click_y = evm->y ;
	my->click_x = evm->x ;
	if (my->click_n == 1 && my->show_edit == TRUE) {
		switch (my->edit_mode) {
			case EDIT_ADD_CURVE : 
				refresh_area (my->area1);
				break;
			case EDIT_MOVE_CURVE : 
				move_curve (&my->curve_infos, my->click_x - my->last_x, 
							  my->click_y - my->last_y);
				refresh_area (my->area1); 
				break;
			case EDIT_REMOVE_CURVE : refresh_area (my->area1); break;
			case EDIT_ADD_CONTROL : 
				refresh_area (my->area1); 
				break;
			case EDIT_MOVE_CONTROL : 
				move_control (&my->curve_infos, my->click_x - my->last_x, 
							  my->click_y - my->last_y);
				refresh_area (my->area1); 
				break;
			case EDIT_REMOVE_CONTROL : refresh_area (my->area1); break;
			case EDIT_MOVE_CLIP : 
				
				move_curve (&my->curve_infos, my->click_x - my->last_x, 
							  my->click_y - my->last_y);
				move_shift (&my->curve_infos, my->click_x - my->last_x, 
							  my->click_y - my->last_y);
				refresh_area(my->area1);
				break;
		}
	}
	
	// CANON
	// calcul et stockage

	if (my->game->state == GS_PLAYING)
		update_canon_angle(my->game, my->click_x, my->click_y);

	refresh_area(my->area1);
	return TRUE;  // evenement traite
}

gboolean on_area1_enter_notify (GtkWidget *area, GdkEvent *event, gpointer data){
	Mydata *my = get_mydata(data);
	gtk_widget_grab_focus (my->area1);
	GdkEventCrossing *evc = &event->crossing;
	printf ("%s: %.1f %.1f\n", __func__, evc->x, evc->y);
	return TRUE;  //  ́evenement trait́e
}

gboolean on_area1_leave_notify (GtkWidget *area, GdkEvent *event, gpointer data){
	GdkEventCrossing *evc = &event->crossing;
	printf ("%s: %.1f %.1f\n", __func__, evc->x, evc->y);
	return TRUE;  //  ́evénement trait ́e
}

void draw_control_labels (cairo_t *cr, PangoLayout *layout, Curve_infos *ci)
{
	font_set_name (layout, "Sans 8");
	cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
	
	int i, j;
	for (i = 0; i < ci->curve_list.curve_count ; i++)
		for (j = 0; j < ci->curve_list.curves[i].control_count ; j++)
			font_draw_text (cr, layout, FONT_TC, ci->curve_list.curves[i].controls[j].x, ci->curve_list.curves[i].controls[j].y -20, "%d", j);
}

/* Bezier */

void draw_bezier_polygon_open (cairo_t *cr, Curve_infos *ci)
{
	int i, j;
	Control bez_points[4];
	for (j = 0; j < ci->curve_list.curve_count ; j++)
		for (i = 0; i < (ci->curve_list.curves[j].control_count - 3) ; i++)
		{
			compute_bezier_points_open (&ci->curve_list.curves[j], i, bez_points);
			cairo_set_source_rgb (cr, 0, 1.0, 0);
			cairo_move_to (cr, bez_points[0].x, bez_points[0].y);
			cairo_line_to (cr, bez_points[1].x, bez_points[1].y);
			cairo_stroke (cr);
			cairo_move_to (cr, bez_points[2].x, bez_points[2].y);
			cairo_line_to (cr, bez_points[3].x, bez_points[3].y);
			cairo_stroke (cr);
		}
}

void draw_bezier_curve (cairo_t *cr, Control bez_points[4], double theta)
{
	double bx[4], by[4];
	int i;
	double t;
	for (i = 0; i < 4; i++)
	{
		bx[i] = bez_points[i].x;
		by[i] = bez_points[i].y;
	}
	
	cairo_move_to (cr, compute_bezier_cubic (bx, 0.0), compute_bezier_cubic (by, 0.0));
	for (t = theta; t < 1.0; t += theta)
	{
		cairo_line_to (cr, compute_bezier_cubic (bx, t), compute_bezier_cubic (by, t));
	}
	cairo_stroke (cr);
}

void draw_bezier_curves_open (cairo_t *cr, Curve_infos *ci, double theta)
{
	int i, j;
	Control bez_points[4];
	for (j = 0; j < ci->curve_list.curve_count ; j++)
		for (i = 0; i < (ci->curve_list.curves[j].control_count - 3) ; i++)
		{
			compute_bezier_points_open (&ci->curve_list.curves[j], i, bez_points);
			cairo_set_source_rgb (cr, 0.5, 0.0, 0.5);
			draw_bezier_curve (cr, bez_points, theta);
		}
}

void draw_bezier_curves_close (cairo_t *cr, Curve_infos *ci, double theta)
{
	int i, j;
	Control bez_points[4];
	for (j = 0; j < ci->curve_list.curve_count ; j++)
		for (i = 0; i < (ci->curve_list.curves[j].control_count) ; i++)
		{
			compute_bezier_points_close (&ci->curve_list.curves[j], i, bez_points);
			cairo_set_source_rgb (cr, 0.5, 0.0, 0.5);
			draw_bezier_curve (cr, bez_points, theta);
		}
}

void draw_bezier_curves_prolong (cairo_t *cr, Curve_infos *ci, double theta){
	Control bez_points[4];
	cairo_set_source_rgb (cr, 0.5, 0.0, 0.5);        
	for (int j = 0; j < ci->curve_list.curve_count; j++){
		Curve *curve = &ci->curve_list.curves[j];
		if (curve->control_count <= 3) continue;
		
		compute_bezier_points_prolong_first (curve, bez_points);
		draw_bezier_curve (cr, bez_points, theta);
		
		for (int i = 0; i < (curve->control_count - 3) ; i++){
			compute_bezier_points_open (curve, i, bez_points);
			draw_bezier_curve (cr, bez_points, theta);
		}
		compute_bezier_points_prolong_last (curve, bez_points);
		draw_bezier_curve (cr, bez_points, theta);
	}    
}

void generate_bezier_path (cairo_t *cr, Control bez_points[4], double theta, int is_first)
{
	double bx[4], by[4];
	int i;
	double t;
	for (i = 0; i < 4; i++)
	{
		bx[i] = bez_points[i].x;
		by[i] = bez_points[i].y;
	}
	
	if (is_first == 1)
		cairo_move_to (cr, compute_bezier_cubic (bx, 0.0), compute_bezier_cubic (by, 0.0));
	for (t = theta; t < 1.0; t += theta)
	{
		cairo_line_to (cr, compute_bezier_cubic (bx, t), compute_bezier_cubic (by, t));
	}
}

void draw_bezier_curves_fill (cairo_t *cr, Curve_infos *ci, double theta)
{
	
	int i, j;
	Control bez_points[4];
	for (j = 0; j < ci->curve_list.curve_count ; j++)
		for (i = 0; i < (ci->curve_list.curves[j].control_count) ; i++)
		{
			compute_bezier_points_close (&ci->curve_list.curves[j], i, bez_points);
			cairo_set_source_rgb (cr, 0.5, 0.0, 0.5);
			if (i == 0)
				generate_bezier_path (cr, bez_points, theta, TRUE);
			else
				generate_bezier_path (cr, bez_points, theta, FALSE);
		}
	cairo_fill(cr);
}

void draw_bezier_curves_clip(cairo_t *cr, Curve_infos *ci, double theta, Mydata *my)
{
    int i, j;
    Control bez_points[4];
    for (j = 0; j < ci->curve_list.curve_count ; j++)
    {
        Curve *curve = &ci->curve_list.curves[j];
        for (i = 0; i < (curve->control_count) ; i++)
        {
            if(my->pixbuf2 != NULL)
                gdk_cairo_set_source_pixbuf(cr,my->pixbuf2,ci->curve_list.curves[j].shift_x,ci->curve_list.curves[j].shift_y);
            else
                cairo_set_source_rgb (cr, 0.5, 0.0, 0.5);
            
            compute_bezier_points_close (curve, i, bez_points);
            if(i==0)
                generate_bezier_path (cr, bez_points, theta, TRUE);
            else
                generate_bezier_path (cr, bez_points, theta, FALSE);
        }
        cairo_fill (cr);
    }
}

gboolean on_area1_draw (GtkWidget *area, cairo_t *cr, gpointer data){   
	
	Mydata *my = get_mydata(data);
	if((my->pixbuf2 != NULL) && ( my->bsp_mode != BSP_CLIP ) ){
        int pix_width = gdk_pixbuf_get_width(my->pixbuf2);
        int pix_height = gdk_pixbuf_get_height(my->pixbuf2);
        gdk_cairo_set_source_pixbuf(cr,my->pixbuf2,0,0);
        if (my->clip_image == FALSE) {
            cairo_rectangle (cr, 0.0, 0.0, pix_width, pix_height);
            cairo_fill (cr);
        }
    }
    
    PangoLayout *layout = pango_cairo_create_layout (cr);
    
    //draw_bg (cr, my);
	if (my->game->state != GS_EDIT)
	{
		draw_score (cr, my);
		draw_train_tracks (cr, my);
		draw_canon (cr, my);
		draw_shots (cr, my);
		draw_train_marbles (cr, my);
		
	}
	else
	{
		draw_control_polygons (cr, &my->curve_infos);
		draw_control_labels (cr, layout, &my->curve_infos);
		draw_bezier_polygon_open (cr, &my->curve_infos);
		draw_bezier_curves_prolong (cr, &my->curve_infos, 0.1);
	}
	
	g_object_unref (layout);
    
    return TRUE;
}
/*
 *     on_timeout1 :
        progress_game_next_step // tout se passe ici, cf module game
        refresh_area
*/



void area1_init (gpointer user_data){
	Mydata *my = get_mydata(user_data);	
								 
	my->area1 = gtk_drawing_area_new ();

	g_signal_connect (my->area1, "draw", G_CALLBACK (on_area1_draw), my);
	
	g_signal_connect (my->area1, "key-press-event", G_CALLBACK (on_area1_key_press), my);
	g_signal_connect (my->area1, "key-release-event", G_CALLBACK (on_area1_key_release), my);
	g_signal_connect (my->area1, "button-press-event", G_CALLBACK (on_area1_button_press), my);
	g_signal_connect (my->area1, "button-release-event", G_CALLBACK (on_area1_button_release), my);
	g_signal_connect (my->area1, "motion-notify-event", G_CALLBACK (on_area1_motion_notify), my);
	g_signal_connect (my->area1, "enter-notify-event", G_CALLBACK (on_area1_enter_notify), my);
	g_signal_connect (my->area1, "leave-notify-event", G_CALLBACK (on_area1_leave_notify), my);


	gtk_widget_add_events  (my->area1,GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK |
							GDK_FOCUS_CHANGE_MASK |
							GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
							GDK_POINTER_MOTION_MASK |
							GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK );
	
	gtk_widget_set_can_focus (my->area1, TRUE);
}


