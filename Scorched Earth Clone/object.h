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
	void setX(float x);
	void setY(float y);
};

class Projectile : public Object {
protected:
	float vertVelocity, horVelocity;
	float x0, y0;
	int t;
	int power;
public:
	void updateTime();
	void updateVelocity(int wind);
	void updateX();
	void updateY();
	bool detectHit(Terrain terrain);
	void draw();
	Projectile(float x, float y, float degree, float power);
};

class Tank : public Object {
protected:
	int a, b, i;
	ALLEGRO_BITMAP *image;
	ALLEGRO_COLOR color;

public:
	int getA();
	int getB();
	int getI();
	void setA(int a);
	void setB(int b);
	void load(int i);
	float calculateDegree(int mouseX, int mouseY);
	Projectile* shoot(int mouseX, int mouseY, int power);
	void updateBarrel(int a, int b);
	void drawBarrel();
	void draw();
	void drawPower(int i);
	bool isHit(Projectile p);
	void drawHitbox();
	void drawMenu(int x, int y);
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
	Projectile* aim();
};