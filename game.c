#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "game.h"
#include "mydata.h"
#include "curve.h"

/*
 * 0 orange
 * 1 pomme
 * 2 pasteque
*/

void sample_curve_to_track (Curve *curve, Track *track, double theta)
{
  Control bez_points[4];
  int ind = 0;

  if (curve->control_count < 3) return;

  compute_bezier_points_prolong_first (curve, bez_points);
  sample_bezier_curve (bez_points, theta, 
    track->sample_x, track->sample_y, &ind, SAMPLE_MAX, 1);

  for (int k = 0; k < curve->control_count-3; k++) {
    compute_bezier_points_open (curve, k, bez_points);
    sample_bezier_curve (bez_points, theta, 
      track->sample_x, track->sample_y, &ind, SAMPLE_MAX, 0);
  }

  compute_bezier_points_prolong_last (curve, bez_points);
  sample_bezier_curve (bez_points, theta, 
    track->sample_x, track->sample_y, &ind, SAMPLE_MAX, 0);

  track->sample_count = ind;
}

void do_vector_product (double xu, double yu, double zu, double xv, double yv, double zv,
	double *x, double *y, double *z)
{
	*x = yu*zv - yv*zu;
	*y = xv*zu - xu*zv;
	*z = xu*yv - xv*yu;
}

void update_canon_angle(Game *game, double sx, double sy)
{
	double vx, vy, n, a;
	vx = sx - game->canon.cx;
	vy = sy - game->canon.cy;
	n = sqrt(vx*vx + vy*vy);
	a = acos (vx / n);
	if ((vy / n) < 0)
		a *= -1;
	game->canon.angle = a;
}

void shoot_ammo (Game *game, double sx, double sy)
{
	int count = game->shot_list.shot_count;
	if ( (count < 10) && (game->canon.reload < game->canon.reload_time) )
	{
		game->shot_list.shots[count].dx = cos (game->canon.angle);
		game->shot_list.shots[count].dy = sin (game->canon.angle);
		game->shot_list.shots[count].x = game->canon.cx + game->shot_list.shots[count].dx*50;
		game->shot_list.shots[count].y = game->canon.cy + game->shot_list.shots[count].dy*50;
		
		game->shot_list.shots[count].color = game->canon.ammo1;
		
		game->shot_list.shot_count++;
		
		game->canon.reload = game->canon.reload_time;
		prepare_ammo (game);
	}
	
}

void prepare_ammo (Game *game)
{
	game->canon.ammo1 = game->canon.ammo2;
	game->canon.ammo2 = rand()%3;
}

void move_trains_one_step (Game *game)
{
	int i, j;
	double dist = 1.5;
	double diametre = 15;
	int count = game->track_list.track_count;
	
	int marble_count, first_visible;
	double result;
	double xB, yB;
	int dist_x, dist_y, dist_tot;
	for (i = 0; i < count; i++)
	{
		marble_count = game->track_list.tracks[i].marble_count;
		first_visible = game->track_list.tracks[i].first_visible;
		result = compute_distant_point_forward (game->track_list.tracks[i].sample_x, game->track_list.tracks[i].sample_y, 
			game->track_list.tracks[i].marbles[first_visible].t, game->track_list.tracks[i].sample_count, dist, &xB, &yB);
		
		if (result >= 0.0)
		{
			game->track_list.tracks[i].marbles[first_visible].x = xB;
			game->track_list.tracks[i].marbles[first_visible].y = yB;
			game->track_list.tracks[i].marbles[first_visible].t = result;
			if (i > 0)
				printf ("x : %lf y : %lf\n", xB, yB);
		}	
		
		dist_x = game->track_list.tracks[i].marbles[marble_count - 1].x - game->track_list.tracks[i].sample_x[game->track_list.tracks[i].sample_count - 1];
		dist_y = game->track_list.tracks[i].marbles[marble_count - 1].y - game->track_list.tracks[i].sample_y[game->track_list.tracks[i].sample_count - 1];

		if ( sqrt(dist_x*dist_x + dist_y*dist_y) <= diametre)
		{
			dist *= 10;
			//printf ("FINISH\n");
		}
		
		// Pousser les billes apres first_visible
		for (j = first_visible + 1; j < marble_count; j++)
		{
			dist_x = game->track_list.tracks[i].marbles[j].x - game->track_list.tracks[i].marbles[j - 1].x;
			dist_y = game->track_list.tracks[i].marbles[j].y - game->track_list.tracks[i].marbles[j - 1].y;
			dist_tot = dist_x*dist_x + dist_y*dist_y;
			// Arreter des que la bille est plus loin qu'un diametre
			if ( dist_tot < (diametre*2)*(diametre*2))
			{
				// pousser d'un diametre de bille
				result = compute_distant_point_forward (game->track_list.tracks[i].sample_x, game->track_list.tracks[i].sample_y, 
					game->track_list.tracks[i].marbles[j].t, game->track_list.tracks[i].sample_count, dist, &xB, &yB);
				if (result >= 0.0)
				{
					game->track_list.tracks[i].marbles[j].x = xB;
					game->track_list.tracks[i].marbles[j].y = yB;
					game->track_list.tracks[i].marbles[j].t = result;
				}
				
			}
			//else break;
			
		}
		
		// Apparition nouvelles billes
		if ( (game->track_list.tracks[i].marbles[first_visible].x - game->track_list.tracks[i].sample_x[0] > diametre) 
			&& (game->track_list.tracks[i].marbles[first_visible].y - game->track_list.tracks[i].sample_y[0] > diametre)  )
		{
			if (game->track_list.tracks[i].first_visible >= 0)
			{
				printf ("Now %d marbles\n", marble_count - game->track_list.tracks[i].first_visible);
				game->track_list.tracks[i].first_visible--;
				result = compute_distant_point_backward (game->track_list.tracks[i].sample_x, game->track_list.tracks[i].sample_y, 
					game->track_list.tracks[i].marbles[first_visible].t, game->track_list.tracks[i].sample_count, dist, &xB, &yB);
				
				if (result >= 0.0)
				{
					game->track_list.tracks[i].marbles[first_visible].x = xB;
					game->track_list.tracks[i].marbles[first_visible].y = yB;
					game->track_list.tracks[i].marbles[first_visible].t = result;
				}
			}	
		}
	}
}

int check_combo (Game *game, int track_i, int *k, int combo)
{
	int pos = *k;
	Track * track = &game->track_list.tracks[track_i];
	Marble * marbles = track->marbles;
	int color = track->marbles[pos].color;
	
	int group_size = 1;
	
	int cpt_d = pos + 1;
	while (marbles[cpt_d].color == color && cpt_d < track->marble_count)
	{
		group_size++;
		cpt_d++;
	}
	
	int cpt_g = pos - 1;
	while (marbles[cpt_g].color == color && cpt_g >= track->first_visible)
	{
		group_size++;
		cpt_g--;
	}
	
	if (group_size > 2)
	{
		printf ("From %d to %d (%d)\n", (cpt_g + 1), (cpt_g + group_size + 1), 
			(track->marble_count - group_size - cpt_g) );
		memmove ( marbles + cpt_g + 1, marbles + cpt_g + group_size + 1,
			sizeof(Marble)*(track->marble_count - group_size - cpt_g) );
		track->marble_count -= group_size;
	}
	return group_size;
}

void check_collisions (Game *game, int i)
{
	int j, k, l;

	int count = game->track_list.track_count;
	int diametre = 30;

	int marble_count, first_visible;
	double dist = 1.5;
	double dist_x, dist_y, p;
	
	double sx, sy, nsx, nsy; // shot x, shot y, next shot x, next shot y
	double mx, my, nmx, nmy; // marble x, marble y, next marble x, next marble y
	
	double rsx, rsy, rsz; // result shot x, result shot y, result shot z
	double rmx, rmy, rmz; // result marble x, result marble y, result marble z
	double rx, ry, rz; // result x, result y, result z
	double ix, iy; // intersection x, intersection y
	
	for (j = 0; j < count; j++)
	{
		marble_count = game->track_list.tracks[i].marble_count;
		first_visible = game->track_list.tracks[i].first_visible;
		for (k = first_visible; k < marble_count; k++)
		{
			sx = game->shot_list.shots[i].x;
			sy = game->shot_list.shots[i].y;
			mx = game->track_list.tracks[j].marbles[k].x;
			my = game->track_list.tracks[j].marbles[k].y;
			dist_x = mx - sx;
			dist_y = my - sy;
			// Verification de la distance entre les deux billes
			if ( sqrt(dist_x*dist_x + dist_y*dist_y) < diametre/2)
			{
				printf ("Collision at : %lf %lf\n", dist_x, dist_y);
				
				nsx = sx + game->shot_list.shots[i].dx*SHOT_SPEED;
				nsy = sy + game->shot_list.shots[i].dy*SHOT_SPEED;
				
				compute_distant_point_forward (game->track_list.tracks[j].sample_x, game->track_list.tracks[j].sample_y, 
					game->track_list.tracks[j].marbles[k].t, game->track_list.tracks[j].sample_count, dist, &nmx, &nmy);
				
				do_vector_product (mx, my, 1, nmx, nmy, 1, &rmx, &rmy, &rmz);
				do_vector_product (sx, sy, 1, nsx, nsy, 1, &rsx, &rsy, &rsz);
				do_vector_product (rsx, rsy, rsz, rmx, rmy, rmz, &rx, &ry, &rz);
				if (rz  != 0)
				{
					ix = rx/rz;
					iy = ry/rz;
					
					if (mx != nmx)
						p = (ix - mx)/(nmx - mx);
					else
						p = (iy - my)/(nmy - my);
					
					double xB, yB;
					int result;
					if (p >= 0) // On insere la marble avant
					{
						if (k == marble_count - 1)
						{
							result = compute_distant_point_forward (game->track_list.tracks[j].sample_x, game->track_list.tracks[j].sample_y, 
							game->track_list.tracks[j].marbles[k].t, game->track_list.tracks[j].sample_count, diametre, &xB, &yB);
					
							if (result >= 0.0)
							{
								game->track_list.tracks[i].marble_count++;
								game->track_list.tracks[i].marbles[k + 1].x = xB;
								game->track_list.tracks[i].marbles[k + 1].y = yB;
								game->track_list.tracks[i].marbles[k + 1].t = result;

							}
							game->track_list.tracks[j].marbles[k + 1].color = game->shot_list.shots[i].color;
							
							memmove (game->shot_list.shots+i, game->shot_list.shots+i+1, 
								sizeof(Shot)*(game->shot_list.shot_count-1-i));
							game->shot_list.shot_count--;
						}
						else
						{
							Marble firstMarble = game->track_list.tracks[j].marbles[k + 1];
							for (l = k + 1; l < marble_count; l++)
							{
								result = compute_distant_point_forward (game->track_list.tracks[j].sample_x, game->track_list.tracks[j].sample_y, 
									game->track_list.tracks[j].marbles[l].t, game->track_list.tracks[j].sample_count, diametre, &xB, &yB);
						
								if (result >= 0.0)
								{
									game->track_list.tracks[i].marbles[l].x = xB;
									game->track_list.tracks[i].marbles[l].y = yB;
									game->track_list.tracks[i].marbles[l].t = result;

								}
							}
							
							memmove (game->track_list.tracks[j].marbles + k + 2, game->track_list.tracks[j].marbles + k + 1, 
								sizeof (Marble)*(marble_count - k + 1));
							game->track_list.tracks[j].marble_count++;

							firstMarble.color = game->shot_list.shots[i].color;
							game->track_list.tracks[j].marbles[k + 1] = firstMarble;
							memmove (game->shot_list.shots + i, game->shot_list.shots + i + 1, 
								sizeof(Shot)*(game->shot_list.shot_count - 1 - i));
							game->shot_list.shot_count--;
						}
						
					}
					else // On insere apres
					{
						Marble firstMarble = game->track_list.tracks[j].marbles[k];
						firstMarble.color = game->shot_list.shots[i].color;
						
						for (l = k; l < marble_count; l++)
						{
							result = compute_distant_point_forward (game->track_list.tracks[j].sample_x, game->track_list.tracks[j].sample_y, 
								game->track_list.tracks[j].marbles[l].t, game->track_list.tracks[j].sample_count, diametre, &xB, &yB);
					
							if (result >= 0.0)
							{
								game->track_list.tracks[i].marbles[l].x = xB;
								game->track_list.tracks[i].marbles[l].y = yB;
								game->track_list.tracks[i].marbles[l].t = result;

							}
						}
						game->track_list.tracks[i].marble_count++;
						memmove (game->track_list.tracks[j].marbles + k + 1,game->track_list.tracks[j].marbles + k, 
							sizeof (Marble)*(marble_count - k));
						
						game->track_list.tracks[i].marbles[k] = firstMarble;
						
						memmove (game->shot_list.shots + i, game->shot_list.shots + i + 1, 
							sizeof(Shot)*(game->shot_list.shot_count - i - 1));
						game->shot_list.shot_count--;
						
					}
					
				//Combo
				int group;
				int combo = 1;
				//do
				//{
					group = check_combo (game, j, &k, combo);
					
					printf ("Combo = %d Group = %d\n", combo, group);
					combo++;
				//}
				//while (group > 2);
				}
				
			}
		}
	}
}

void move_shot_one_step (Game *game)
{
	int i;
	int count = game->shot_list.shot_count;
	for (i = 0; i < count; i++)
	{
		game->shot_list.shots[i].x += game->shot_list.shots[i].dx*SHOT_SPEED;
		game->shot_list.shots[i].y += game->shot_list.shots[i].dy*SHOT_SPEED;
		check_collisions (game, i);
	}
}

void suppress_far_shot (Game *game, int w, int h)
{
	int i, x, y;
	int count = game->shot_list.shot_count;
	for (i = 0; i < count; i++)
	{
		x = game->shot_list.shots[i].x;
		y = game->shot_list.shots[i].y;
		
		if ( (x < 0) || (y < 0) || (x > w) || (y > h) )
		{
			//printf ("Kill shot : %d\n", i);
			
			memmove (game->shot_list.shots + i, game->shot_list.shots + i + 1, 
				sizeof(Shot)*(game->shot_list.shot_count - i - 1));
			game->shot_list.shot_count--;
			
			printf ("Now %d shot \n", game->shot_list.shot_count);
		}
		
	}
}

void swap_ammo (Game *game)
{
	int tmp = game->canon.ammo1;
	game->canon.ammo1 = game->canon.ammo2;
	game->canon.ammo2 = tmp;
}

void init_track (Game *game, Curve_infos *ci)
{
	//initialiser curve selon niveau
	
	//echantilloner courbe B spline cubique uniforme prolongee
	int count = ci->curve_list.curve_count;
	
	//game->track_list.track_count = 1;
	//Track track_list[count];
	printf ("count : %d\n", count);
	int i, j;
	for (i = 0; i < count; i++)
	{
		sample_curve_to_track (&ci->curve_list.curves[i], &game->track_list.tracks[i], 0.1);
		game->track_list.tracks[i].marble_count = 30;
		for (j = 0; j < game->track_list.tracks[i].marble_count; j++)
		{
			game->track_list.tracks[i].marbles[j].t = 0.0;
			game->track_list.tracks[i].marbles[j].x = game->track_list.tracks[i].sample_x[0];
			game->track_list.tracks[i].marbles[j].y = game->track_list.tracks[i].sample_y[0];
			game->track_list.tracks[i].marbles[j].color = g_rand_int_range (game->g_rand, 0, 3);
			printf ("New marble %d %lf %lf %d \n", j, game->track_list.tracks[i].marbles[j].x, game->track_list.tracks[i].marbles[j].y,
				game->track_list.tracks[i].marbles[j].color);
		}
		game->track_list.tracks[i].first_visible = game->track_list.tracks[i].marble_count - 1;
		printf ("First visible %d\n", game->track_list.tracks[i].first_visible);
	}
	
}

void progress_game_next_step (Game *game, int w, int h)
{
	suppress_far_shot (game, w, h);
	move_shot_one_step (game);
	//~ process_shots_collisions
	move_trains_one_step (game);
	//~ check_end_of_game
}


