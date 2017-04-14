#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "curve.h"
#include "util.h"
#include "game.h"
#include "mydata.h"
#include "drawings.h"
#include "menus.h"
#include "gui.h"



//Chargement de l'appli

void on_app_activate (GtkApplication* app, gpointer user_data){
	g_object_set (gtk_settings_get_default(),
				"gtk-shell-shows-menubar", FALSE, NULL);
	
	Mydata *my = get_mydata(user_data);
	
	window_init(app, my);	
	menu_init(my);	
	area1_init(my);	
	status_init(my);
	editing_init(my);	
	layout_init(my);		
	win_scale_init(my);
	game_init (my);
	g_timeout_add (20, on_timeout1, my);
	gtk_widget_show_all (my->window);
	gtk_widget_hide (my->frame1);	
}

int main (int argc, char *argv[]){
    GtkApplication *app;
    int status;
    
    srand(time(NULL));
    
    Mydata my;
    init_data(&my);
    app = gtk_application_new (NULL, G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate",G_CALLBACK(on_app_activate), &my);
    status = g_application_run (G_APPLICATION(app), argc, argv);
    g_object_unref (app);
    return status;
}


