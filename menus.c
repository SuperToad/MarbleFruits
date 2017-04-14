#include <gtk/gtk.h>
#include "util.h"
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "drawings.h"
#include "menus.h"

void on_item_load_activate (GtkWidget *widget, gpointer data){
    Mydata *my = get_mydata(data);
    
    GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dialog = gtk_file_chooser_dialog_new ("Load Image",
										  GTK_WINDOW(my->window),
										  action,
										  "Cancel",
										  GTK_RESPONSE_CANCEL,
										  "Open",
										  GTK_RESPONSE_ACCEPT,
										  NULL);
	if (my->current_folder != NULL) 
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER (dialog), my->current_folder);
	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)	{
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);
		set_status(my->status, "Loading image ...");
		g_clear_object(&my->pixbuf1);
		my->pixbuf1 = gdk_pixbuf_new_from_file(filename, NULL);
		if (my->pixbuf1 == NULL) { 
			set_status(my->status, "Loading failed : not an image.");
			//gtk_image_set_from_icon_name (GTK_IMAGE (my->image1), "image-missing",
			//							  GTK_ICON_SIZE_DIALOG);
		}
		else {
			char str[80];
			sprintf(str, "Loading success : image %dx%d.", gdk_pixbuf_get_width(my->pixbuf1),
														 gdk_pixbuf_get_height(my->pixbuf1));
			
			set_status(my->status, str);
			//gtk_widget_set_size_request (my->area1,status gdk_pixbuf_get_width (my->pixbuf1), gdk_pixbuf_get_height (my->pixbuf1));
			my->scale_value = 1.0;
			my->rotate_angle = 0.0;
			apply_image_transforms (my);
		}
		my->current_folder = NULL;
		my->current_folder = gtk_file_chooser_get_current_folder(chooser);	
		g_free(filename);	
	}

	gtk_widget_destroy (dialog);
    
    printf ("Button clicked in window ’%s’\n", my->title);
}

void on_item_quit_activate (GtkWidget *widget, gpointer data){
    Mydata *my = get_mydata(data);
    printf ("Closing window and app\n");
    gtk_widget_destroy(my->window);
}

void on_item_save_level_activate (GtkWidget *widget, gpointer data){
    Mydata *my = get_mydata(data);

	FILE * fp;

	fp = fopen ("./levels/0.txt", "w+");
	int count = my->curve_infos.curve_list.curve_count;
	fprintf(fp, "%d\n", count);
	int i, j, control_count;
	for (i = 0; i < count; i++)
	{
		//fprintf(fp, "%f %f\n", my->curve_infos.curve_list.curves[i].shift_x,  my->curve_infos.curve_list.curves[i].shift_y);
		control_count = my->curve_infos.curve_list.curves[i].control_count;
		fprintf(fp, "%d\n", control_count);
		for (j = 0; j < control_count; j++)
		{
			fprintf(fp, "%f %f\n", my->curve_infos.curve_list.curves[i].controls[j].x,  my->curve_infos.curve_list.curves[i].controls[j].y);
		}
	}

	fclose(fp);
}

void on_item_load_level_activate (GtkWidget *widget, gpointer data){
    Mydata *my = get_mydata(data);
    
    FILE *file = fopen ("./levels/0.txt", "r");
	
	if (file != NULL)
	{
		//Curve curve;
		int curve_count, control_count;
		double x, y;
		if (fscanf(file, "%d", &curve_count) != 1)
			printf ("Error reading file");
		my->game->track_list.track_count = curve_count;
		int i, j;
		for (i = 0; i < curve_count; i++)
		{
			add_curve (&my->curve_infos);
			if (fscanf(file, "%d", &control_count) != 1)
				printf ("Error reading file");
			for (j = 0; j < control_count; j++)
			{
				if (fscanf(file, "%lf %lf", &x, &y) != 1)
					printf ("Error reading file");
				add_control (&my->curve_infos, x, y);
			}
		}
		init_track (my->game, &my->curve_infos);
		curve_count += x + y + control_count;
	}
}

void on_item_about_activate (GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);
	
	char *auteurs[] = {"Les branlos de l'Xtreme <rpzarles@chika.com>", "DUFFAUT Julien <super-toad@hotmail.fr>", "PASTOR Jean-Baptiste <tigerdu13@hotmail.fr>", NULL};
	
	gtk_show_about_dialog(NULL,
			"program-name", my->title,
			"version", "2.4",
			"website", "http://j.mp/optigra",
			"authors", auteurs,
			"logo-icon-name", "face-cool",
			NULL);
}

void on_item_color_activate (GtkWidget *widget, gpointer data){
	Mydata *my = get_mydata(data);
	gint res = 0;
    printf ("Color\n");
    GtkWidget *color_chooser;
    color_chooser = gtk_color_chooser_dialog_new ("Background color", NULL);
    res = gtk_dialog_run(GTK_DIALOG (color_chooser));
	if (res == GTK_RESPONSE_OK) { 
		GdkRGBA bg_color;
		gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (color_chooser), &bg_color);
		gchar *c;
		c = gdk_rgba_to_string (&bg_color);
		char s[1000];
		sprintf(s, "Selected bg color : %s", c);
		set_status (my->status, s);
		gtk_widget_override_background_color(my->area1, GTK_STATE_FLAG_NORMAL, &bg_color);
		g_free(c);
	}
    gtk_widget_destroy(color_chooser);
}

void on_item_rotate_activate (GtkWidget *widget, gpointer data){
    Mydata *my = get_mydata(data);
    if (my->pixbuf1 != NULL){
		my->rotate_angle = my->rotate_angle + 90;
		if (my->rotate_angle >= 360) 
			my->rotate_angle = my->rotate_angle - 360;
		update_area1_with_transforms (my);
		
		set_status(my->status, "Image rotated.");
	}
	else 
		set_status(my->status, "No image to rotate.");
}

void on_item_scale_activate (GtkWidget *widget, gpointer data){
    Mydata *my = get_mydata(data);	 
	gtk_range_set_value(GTK_RANGE(my->scale1),my->scale_value);
	gtk_window_present (GTK_WINDOW (my->win_scale));
}

void on_item_clip_activate (GtkCheckMenuItem *widget, gpointer data){
    Mydata *my = get_mydata(data);	
    
    my->clip_image = gtk_check_menu_item_get_active (widget);
    if (my->clip_image == TRUE)
        set_status(my->status, "Clipping is on");
    else
        set_status(my->status, "Clipping is off");
        
    refresh_area (my->area1);
}

void on_item_edit_activate (GtkCheckMenuItem *widget, gpointer data){
    Mydata *my = get_mydata(data);	
    
    my->show_edit = gtk_check_menu_item_get_active (widget);
    if (my->show_edit == TRUE) {
        set_status(my->status, "Editing mode is on");
        my->game_mode = EDIT;
    	gtk_widget_show (my->frame1);
    }
    else {
        set_status(my->status, "Playing mode is on");
        my->game_mode = PLAY;
        gtk_widget_hide (my->frame1);
    }
    
    
    refresh_area (my->area1);
}

void menu_init (gpointer user_data){
	Mydata *my = get_mydata(user_data);
	
	GtkWidget *item_file, *item_tools, 
			  *item_help, *sub_file, *item_load, *item_quit, *sub_tools,
			  *item_rotate, *item_color, *item_scale, *sub_help, *item_about,
			  *item_clip, *item_edit, *item_save_level, *item_load_level;
	
	my->menu_bar = gtk_menu_bar_new();	
	
	//Menu	
	item_file = gtk_menu_item_new_with_label ("Game");
	item_tools = gtk_menu_item_new_with_label ("Level");
	item_help = gtk_menu_item_new_with_label ("Help");
	
	gtk_menu_shell_append(GTK_MENU_SHELL(my->menu_bar), item_file);
	gtk_menu_shell_append(GTK_MENU_SHELL(my->menu_bar), item_tools);
	gtk_menu_shell_append(GTK_MENU_SHELL(my->menu_bar), item_help);
	
	//File / Game
	sub_file = gtk_menu_new ();
	
	item_load = gtk_menu_item_new_with_label ("Load BG");
	item_quit = gtk_menu_item_new_with_label ("Quit");
	
	item_load_level = gtk_menu_item_new_with_label ("Start");
	
	g_signal_connect (item_load, "activate",
					  G_CALLBACK(on_item_load_activate), my);
	g_signal_connect (item_quit, "activate",
					  G_CALLBACK(on_item_quit_activate), my);
	
	g_signal_connect (item_load_level, "activate",
					  G_CALLBACK(on_item_load_level_activate), my);
					  	
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_file), item_load);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_file), item_load_level);
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_file), item_quit);
	
	//Tools / Level
	sub_tools = gtk_menu_new ();
	
	item_save_level = gtk_menu_item_new_with_label ("Save");
	item_rotate = gtk_menu_item_new_with_label ("Rotate");
	item_color = gtk_menu_item_new_with_label ("Bg Color");
	item_scale = gtk_menu_item_new_with_label ("Scale");
	item_clip = gtk_check_menu_item_new_with_label ("Clip");
	item_edit = gtk_check_menu_item_new_with_label ("Edit");

	g_signal_connect (item_rotate, "activate",
					  G_CALLBACK(on_item_rotate_activate), my);
	g_signal_connect (item_color, "activate",
					  G_CALLBACK(on_item_color_activate), my);
	g_signal_connect (item_scale, "activate",
					  G_CALLBACK(on_item_scale_activate), my);				  
	g_signal_connect (item_clip, "activate",
					  G_CALLBACK(on_item_clip_activate), my);			  
	g_signal_connect (item_edit, "activate",
					  G_CALLBACK(on_item_edit_activate), my);
	g_signal_connect (item_save_level, "activate",
					  G_CALLBACK(on_item_save_level_activate), my);				  
					  						
	//gtk_menu_shell_append(GTK_MENU_SHELL(sub_tools), item_rotate);
	//gtk_menu_shell_append(GTK_MENU_SHELL(sub_tools), item_color);
	//gtk_menu_shell_append(GTK_MENU_SHELL(sub_tools), item_scale);
	//gtk_menu_shell_append(GTK_MENU_SHELL(sub_tools), item_clip);
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_tools), item_edit);
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_file), item_save_level);
	
	//Help
	sub_help = gtk_menu_new ();
	
	item_about = gtk_menu_item_new_with_label ("About");
	
	g_signal_connect (item_about, "activate",
					  G_CALLBACK(on_item_about_activate), my);	
					  
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_help), item_about);
	
	//Sub-menus	
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_file), sub_file);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_tools), sub_tools);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_help), sub_help);	
} 
