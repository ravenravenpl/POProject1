#pragma once

#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "terrain.h"
#define MAX_WIDTH 800
#define MAX_HEIGHT 600

class Object {
protected:
	float x, y;
public:
	float getX();
	float getY();
};

class Projectile : public Object {
protected:
	float vertVelocity, horVelocity;
	float x0, y0;
	int t;
	int power;
public:
	void updateTime();
	void updateVelocity();
	void updateGravity();
	void move();
	bool detectHit(Terrain terrain);
	void draw();
	Projectile(float x, float y, float degree, float power);
};

class Tank : public Object {
protected:
	int HP, a, b, kulax, kulay;
	//float degree;
	ALLEGRO_BITMAP *image;
	ALLEGRO_BITMAP *pocisk;
	ALLEGRO_COLOR color;
	bool stan;

public:
	int getA();
	int getB();
	void load(int i);
	//void place(int x, int y);
	float calculateDegree(int mouseX, int mouseY);
	Projectile* shoot(int mouseX, int mouseY, int power);
	void damage();
	void getPos();
	void updateBarrel(int a, int b);
	void drawBarrel();
	void draw();
	void drawPower(int i);
	bool isHit(Projectile p);
	void drawHitbox();
	//void shoot();
};

class Player : public Tank {
public:
	Player();
	void place(Terrain* terrain);
};

class Enemy : public Tank {
public:
	Enemy();
	void place(Terrain* terrain);
	void aim();
};