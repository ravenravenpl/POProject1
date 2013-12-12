#pragma once

#include <iostream>
#include <allegro5\allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5\allegro_image.h>

class Object {
protected:
	float x, y;
public:
	float getX();
	float getY();
};

class Projectile {
protected:
	float vertVelocity, horVelocity;
public:
	void updateVelocity();
	void move();
	bool detectHit();
};

class Tank {
protected:
	int HP, degree_x, degree_y, power, a, b,bla;
	ALLEGRO_BITMAP *image;

public:
	Projectile shoot();
	void damage();
	void getPos();
	void czolg(int x, int y, int a, int b);
};