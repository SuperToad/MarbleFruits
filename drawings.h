#include <gtk/gtk.h>

#ifndef DRAWINGS_H
#define DRAWINGS_H

void apply_image_transforms (Mydata *data);
void update_area1_with_transforms (Mydata *data);
void draw_control_labels (cairo_t *cr, PangoLayout *layout, Curve_infos *ci);
void draw_control_polygons (cairo_t *cr, Curve_infos *ci);
void draw_bezier_polygon_open (cairo_t *cr, Curve_infos *ci);
void area1_init (gpointer user_data);

void draw_bezier_curve (cairo_t *cr, Control bez_points[4], double theta);
void draw_bezier_curves_open (cairo_t *cr, Curve_infos *ci, double theta);
void draw_bezier_curves_close (cairo_t *cr, Curve_infos *ci, double theta);
void draw_bezier_curves_prolong (cairo_t *cr, Curve_infos *ci, double theta);
void generate_bezier_path (cairo_t *cr, Control bez_points[4], double theta, int is_first);
void draw_bezier_curves_fill (cairo_t *cr, Curve_infos *ci, double theta);
void draw_bezier_curves_clip (cairo_t *cr, Curve_infos *ci, double theta, Mydata *my);

void draw_canon (cairo_t *cr, Mydata *my);
gboolean on_timeout1 (gpointer data);

#endif /* DRAWINGS_H */
