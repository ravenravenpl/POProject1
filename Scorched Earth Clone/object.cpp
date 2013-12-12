#include "object.h"
#include <time.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5\allegro_image.h>

float Object::getX() {
	return this->x;
}

float Object::getY() {
	return this->y;
}

void Tank::czolg(int x, int y, int pos_x, int pos_y){
	image = al_load_bitmap("1czerwony.bmp");
	if (pos_y <= (y + 8)){
		a = 40 * (pos_x - (x + 34)) / sqrt((pos_x - (x + 34))*(pos_x - (x + 34)) + (pos_y - (y + 8))*(pos_y - (y + 8)));
		b = 40 * (pos_y - (y + 8)) / sqrt((pos_x - (x + 34))*(pos_x - (x + 34)) + (pos_y - (y + 8))*(pos_y - (y + 8)));
	}
	al_draw_bitmap(image, x, y, 0);
	al_draw_line(x + 34, y + 8, x + 34 + a, y + 8 + b, al_map_rgb(254, 39, 37), 4);
}
