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
	
	char s[50];
	sprintf(s, "./levels/%d.txt", my->game->current_level);

	fp = fopen (s, "w+");
	int count = my->curve_infos.curve_list.curve_count;
	fprintf(fp, "%d\n", count);
	int i, j, control_count;
	for (i = 0; i < count; i++)
	{
		control_count = my->curve_infos.curve_list.curves[i].control_count;
		fprintf(fp, "%d\n", control_count);
		for (j = 0; j < control_count; j++)
		{
			fprintf(fp, "%f %f\n", my->curve_infos.curve_list.curves[i].controls[j].x,  my->curve_infos.curve_list.curves[i].controls[j].y);
		}
	}

	fclose(fp);
}

void load_bg (Mydata *my)
{
	char *filename;
	int bg = my->game->current_level%3;
	switch (bg)
	{
		case 0 :
			filename = "./images/BG_sky.png";
			break;
		case 1 :
			filename = "./images/BG_sunset.png";
			break;
		case 2 :
			filename = "./images/BG_night.png";
			break;
		default :
			filename = "./images/BG_sky.png";
			break;
	}
	
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
		my->scale_value = 3.5;
		my->rotate_angle = 0.0;
		apply_image_transforms (my);
	}
	
	
}

void on_item_load_level_activate (GtkWidget *widget, gpointer data){
    Mydata *my = get_mydata(data);
    
    char s[50];
	sprintf(s, "./levels/%d.txt", my->game->current_level);
    
    FILE *file = fopen (s, "r");
	
	if (file != NULL)
	{
		int i, j, n;
		// Supression curves courantes
		for (i = 0; i < my->game->track_list.track_count; i++)
		{
			n = find_control (&my->curve_infos, my->curve_infos.curve_list.curves[i].controls[0].x, my->curve_infos.curve_list.curves[i].controls[0].y);
			if (n == 0) remove_curve (&my->curve_infos);
		}
		
		// Ajout curves et controls
		int curve_count, control_count;
		double x, y;
		if (fscanf(file, "%d", &curve_count) != 1)
			printf ("Error reading file");
		my->game->track_list.track_count = curve_count;
		
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
		curve_count += x + y + control_count;
		
		// Chargement du niveau
		load_bg (my);
		set_status(my->status, "New level starting.");
		init_track (my->game, &my->curve_infos);
	}
}

void on_item_new_level_activate (GtkWidget *widget, gpointer data){
    Mydata *my = get_mydata(data);
	
	my->game->current_level = my->game->level_list.level_count;
	my->game->level_list.level_count++;
	
	char str[50];
	sprintf(str, "New level : %d.", my->game->level_list.level_count);
	set_status(my->status, str);
	
	//Supression curves courantes
	int i, n;
	for (i = 0; i < my->game->track_list.track_count; i++)
	{
		n = find_control (&my->curve_infos, my->curve_infos.curve_list.curves[i].controls[0].x, my->curve_infos.curve_list.curves[i].controls[0].y);
		if (n == 0) remove_curve (&my->curve_infos);
	}
	my->game->track_list.track_count = 0;
}

void on_item_about_activate (GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);
	
	char *auteurs[] = {"DUFFAUT Julien <julien.duffaut@etu.univ-amu.fr>", "PASTOR Jean-Baptiste <jb.pastor@outlook.fr>", NULL};
	
	gtk_show_about_dialog(NULL,
			"program-name", my->title,
			"version", "2.4",
			"website", "http://j.mp/optigra",
			"authors", auteurs,
			"logo-icon-name", "face-cool",
			NULL);
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
        my->game->state = GS_EDIT;
    	gtk_widget_show (my->frame1);
    }
    else {
        set_status(my->status, "Playing mode is on");
        my->game->state = GS_PLAYING;
        gtk_widget_hide (my->frame1);
    }
    
    
    refresh_area (my->area1);
}

void on_item_pause_activate (GtkCheckMenuItem *widget, gpointer data){
    Mydata *my = get_mydata(data);	
	if ( (my->game->state != GS_WON) && (my->game->state != GS_LOST) && (my->game->state != GS_HELLO) )
	{
		my->game->state =(my->game->state == GS_PLAYING)?  GS_PAUSE : GS_PLAYING;
	}
	
    refresh_area (my->area1);
}

void menu_init (gpointer user_data){
	Mydata *my = get_mydata(user_data);
	
	GtkWidget *item_file, *item_tools, 
			  *item_help, *sub_file, *item_load, *item_quit, *sub_tools,
			  *sub_help, *item_about,
			  *item_edit, *item_pause,
			  *item_save_level, *item_load_level, *item_new_level;
	
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
	item_pause = gtk_check_menu_item_new_with_label ("Pause");
	
	item_load_level = gtk_menu_item_new_with_label ("Start");
	
	g_signal_connect (item_load, "activate",
					  G_CALLBACK(on_item_load_activate), my);
	g_signal_connect (item_quit, "activate",
					  G_CALLBACK(on_item_quit_activate), my);
	g_signal_connect (item_load_level, "activate",
					  G_CALLBACK(on_item_load_level_activate), my);
	g_signal_connect (item_pause, "activate",
					  G_CALLBACK(on_item_pause_activate), my);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_file), item_load);
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_file), item_load_level);
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_file), item_pause);
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_file), item_quit);
	
	//Tools / Level
	sub_tools = gtk_menu_new ();
	
	item_save_level = gtk_menu_item_new_with_label ("Save");
	item_new_level = gtk_menu_item_new_with_label ("New");
	item_edit = gtk_check_menu_item_new_with_label ("Edit");

	g_signal_connect (item_save_level, "activate",
					  G_CALLBACK(on_item_save_level_activate), my);
	g_signal_connect (item_edit, "activate",
					  G_CALLBACK(on_item_edit_activate), my);	
	g_signal_connect (item_new_level, "activate",
					  G_CALLBACK(on_item_new_level_activate), my);				  
					  						
					  						
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_tools), item_edit);
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_tools), item_new_level);
	gtk_menu_shell_append(GTK_MENU_SHELL(sub_tools), item_save_level);
	
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
