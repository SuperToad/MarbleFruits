#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"



void refresh_area (GtkWidget *area){
	GdkWindow *win = gtk_widget_get_window (area);
	if (win == NULL) return;
	gdk_window_invalidate_rect (win, NULL, FALSE);
}


// Mise à jour de la barre de statut

void set_status (GtkWidget* status, const char *msg){
	//Mydata *my = get_mydata(data);
	gtk_statusbar_pop (GTK_STATUSBAR (status), 0); 
	gtk_statusbar_push (GTK_STATUSBAR (status), 0, msg);
}
