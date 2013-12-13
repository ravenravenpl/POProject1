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

void Tank::updateBarrel(int pos_x, int pos_y){
	// wektorowa magia obliczajaca nam koniec lufy czolgu, która podaza za myszka
	if (pos_y <= (y + 8)){
		a = 40 * (pos_x - (x + 34)) / sqrt((pos_x - (x + 34))*(pos_x - (x + 34)) + (pos_y - (y + 8))*(pos_y - (y + 8)));
		b = 40 * (pos_y - (y + 8)) / sqrt((pos_x - (x + 34))*(pos_x - (x + 34)) + (pos_y - (y + 8))*(pos_y - (y + 8)));
	} 
	al_draw_line(this->x + 34, this->y + 8, this->x + 34 + a, this->y + 8 + b, this->color, 4);
}

void Tank::load(int i) {
	switch (i)
	{
	case 1:
		image = al_load_bitmap("1czerwony.bmp");
		this->color = al_map_rgb(254, 39, 37);
		break;
	case 2:
		image = al_load_bitmap("1zielony.bmp");
		this->color = al_map_rgb(37, 254, 39);
		break;
	case 3:
		image = al_load_bitmap("1zolty.bmp");
		this->color = al_map_rgb(254, 211, 37);
		break;
	case 4:
		image = al_load_bitmap("1niebieski.bmp");
		this->color = al_map_rgb(38, 209, 255);
		break;
	default:
		image = al_load_bitmap("1niebieski.bmp");
		this->color = al_map_rgb(38, 209, 255);
		break;
	}
}

void Tank::draw() {
	al_draw_bitmap(image, this->x, this->y, 0);
}

void Tank::place(int x, int y) {
	this->x = x;
	this->y = y-27;
}

void Tank::shoot(){

}