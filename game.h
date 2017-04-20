#include <gtk/gtk.h>

#ifndef GAME_H
#define GAME_H


#include "curve.h"

typedef enum { GS_HELLO, GS_PLAYING, GS_PAUSE, GS_WON, GS_LOST } Game_state;
typedef enum { TS_INTRO, TS_NORMAL, TS_COMBO2, TS_COMBO3} Track_state;

#define SHOT_MAX       10
#define SHOT_SPEED      10
#define TRACK_MAX      10
#define MARBLE_MAX    200
#define SAMPLE_MAX   1000 
#define LEVEL_MAX      10
#define SAMPLE_THETA    0.05

typedef struct {
  double cx, cy;    // centre canon
  double angle;     // en radians
  int ammo1, ammo2;
  int reload;
  int reload_time;
} Canon;

typedef struct {
  double x, y;      // coordonnées centre
  double dx, dy;    // vecteur déplacement
  int color;
} Shot;

typedef struct {
  int shot_count;
  Shot shots[SHOT_MAX];
} Shot_list;

typedef struct {
  double x, y;      // coordonnées centre
  double t;         // paramètre dans l'échantillonnage
  int color;
  int is_combo_end; // ou encore, facteur vitesse et direction ?
  int step_explode;
} Marble;

typedef struct {
  int    sample_count;          // échantillonnage courbe
  double sample_x[SAMPLE_MAX], 
         sample_y[SAMPLE_MAX];
  int marble_count;
  int first_visible;
  Marble marbles[MARBLE_MAX];
  Track_state state;
} Track;

typedef struct {
  int track_count;
  Track tracks[TRACK_MAX];
} Track_list;

typedef struct {
  Curve_infos curve_infos;
  double canon_x, canon_y;
  int marbles_intro, marbles_total;
} Level;

typedef struct {
  int level_count;
  Level levels[LEVEL_MAX];
} Level_list;

typedef struct {
  Game_state state;
  int current_level;
  int score;
  Canon canon;
  Shot_list shot_list;
  Track_list track_list;
  Level_list level_list;
  GRand *g_rand;
} Game;

void sample_curve_to_track (Curve *curve, Track *track, double theta);

void do_vector_product (double xu, double yu, double zu, double xv, double yv, double zv,
	double *x, double *y, double *z);

void update_canon_angle(Game *game, double sx, double sy);
void shoot_ammo (Game *game, double sx, double sy);
void prepare_ammo (Game *game);
void move_shot_one_step (Game *game);
void suppress_far_shot (Game *game, int w, int h);
void swap_ammo (Game *game);

void move_trains_one_step (Game *game);

void init_track (Game *game, Curve_infos *ci);

void progress_game_next_step (Game *game, int w, int h);

	  
#endif /* GAME_H */
