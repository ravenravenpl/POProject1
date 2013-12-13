#pragma once

#include <iostream>
#include <allegro5\allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5\allegro_image.h>
#define MAX_WIDTH 800
#define MAX_HEIGHT 600

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

class Tank : public Object {
protected:
	int HP, degree_x, degree_y, power, a, b,kulax,kulay;
	ALLEGRO_BITMAP *image;
	ALLEGRO_BITMAP *pocisk;
	ALLEGRO_COLOR color;
	bool stan;

public:
	void load(int i);
	void place(int x, int y);
	//Projectile shoot();
	void damage();
	void getPos();
	void updateBarrel(int a, int b);
	void draw();
	void shoot();
};