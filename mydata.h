#include <gtk/gtk.h>

#ifndef MYDATA_H
#define MYDATA_H

#define MYDATA_MAGIC 0x46EA7E05

enum {EDIT_NONE, EDIT_ADD_CURVE, EDIT_MOVE_CURVE, 
	EDIT_REMOVE_CURVE, EDIT_ADD_CONTROL, EDIT_MOVE_CONTROL, 
	EDIT_REMOVE_CONTROL, EDIT_MOVE_CLIP, EDIT_RESET_CLIP,EDIT_LAST };

enum { BSP_FIRST, BSP_OPEN, BSP_CLOSE, BSP_PROLONG, BSP_FILL, BSP_CLIP, BSP_LAST };

enum { ORANGE, POMME, PASTEQUE };

enum { PLAY, EDIT, PAUSE };

typedef struct {
	GtkWidget *window, *status, *vbox1, *hbox1, *win_scale, *scale1, *area1, 
			  *image1, *menu_bar, *scroll, *frame1, *edit_radios[EDIT_LAST], *bsp_radios[BSP_LAST];
	char *title;
	int win_width, win_height, click_n, clip_image, show_edit, edit_mode, game_mode;
	double scale_value, rotate_angle, click_x, click_y, last_x, last_y;
	unsigned int magic;
	char *current_folder;
	GdkPixbuf *pixbuf1, *pixbuf2, *pixbuf_canon, *pixbuf_canon2;
	Curve_infos curve_infos;
	int bsp_mode;
	int count;
	cairo_surface_t *logo, *canon, *orange, *pomme, *pasteque;
	Game *game;
} Mydata;	  


Mydata *get_mydata (gpointer data);
void init_data(Mydata *data);
void set_edit_mode (Mydata *data, int mode);

void canon_init (Game * game, int win_width, int win_height);
void game_init (Game * game, int win_width, int win_height);
	 
	  
#endif /* MYDATA_H */


