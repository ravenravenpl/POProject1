#include "object.h"

#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5\allegro_image.h>
#define G 9.81

float Object::getX() {
	return this->x;
}

float Object::getY() {
	return this->y;
}

Projectile::Projectile(float x, float y, float degree, float power) {
	this->x = x;
	this->y = y;
	this->y0 = y;
	this->t = 1;
	this->power = power;
	this->horVelocity = sin(degree) <= 0 ? -cos(degree)*power : cos(degree)*power;
	this->vertVelocity = abs(sin(degree))*power;
}

void Projectile::updateTime() {
	this->t++;
}

void Projectile::updateGravity() {
	this->y = this->y0 - (vertVelocity*t - (0.5*G*t*t)*0.01);
}

void Projectile::updateVelocity() {
	this->x = this->x + this->horVelocity;
	//this->y = this->y + this->vertVelocity;
}

bool Projectile::detectHit(Terrain terrain) {
	if (this->y + 4 > MAX_HEIGHT - terrain.getY(this->x)) {
		return true;
	}
	else return false;
}

void Projectile::draw() {
	al_draw_filled_rectangle((this->x)+2, (this->y)+2, (this->x)-2, (this->y)-2, al_map_rgb(150, 0, 0));
}

void Tank::updateBarrel(int pos_x, int pos_y){
	// wektorowa magia obliczajaca nam koniec lufy czolgu, która podaza za myszka
	if (pos_y <= (y + 8)){
		this->a = 40 * (pos_x - (x + 34)) / sqrt((pos_x - (x + 34))*(pos_x - (x + 34)) + (pos_y - (y + 8))*(pos_y - (y + 8)));
		this->b = 40 * (pos_y - (y + 8)) / sqrt((pos_x - (x + 34))*(pos_x - (x + 34)) + (pos_y - (y + 8))*(pos_y - (y + 8)));
	} 
	
}

void Tank::drawBarrel() {
	al_draw_line(this->x + 34, this->y + 8, this->x + 34 + this->a, this->y + 8 + this->b, this->color, 4);
}

void Tank::load(int i) {
	switch (i)
	{
	case 1:
		image = al_load_bitmap("1czerwony.png");
		this->color = al_map_rgb(254, 39, 37);
		break;
	case 2:
		image = al_load_bitmap("1zielony.png");
		this->color = al_map_rgb(37, 254, 39);
		break;
	case 3:
		image = al_load_bitmap("1zolty.png");
		this->color = al_map_rgb(254, 211, 37);
		break;
	case 4:
		image = al_load_bitmap("1niebieski.png");
		this->color = al_map_rgb(38, 209, 255);
		break;
	default:
		image = al_load_bitmap("1niebieski.png");
		this->color = al_map_rgb(38, 209, 255);
		break;
	}
}

void Tank::draw() {
	al_draw_bitmap(image, this->x, this->y, 0);
}

Player::Player() {
	this->a = 39;
	this->b = 0;
}

void Player::place(Terrain* terrain) {
	int r = rand() % (MAX_WIDTH/2-50) + 50;
	this->x = r;
	this->y = MAX_HEIGHT - terrain->getY(r) - 24;
	terrain->flatten(r, r + 64);
}

Enemy::Enemy() {
	this->a = -39;
	this->b = 0;
}

void Enemy::place(Terrain* terrain) {
	int r = rand() % (MAX_WIDTH / 2 - 50) + 350;
	this->x = r;
	this->y = MAX_HEIGHT - terrain->getY(r) - 24;
	terrain->flatten(r, r + 64);
}

float Tank::calculateDegree(int mouseX, int mouseY){
	if (mouseY < this->y + 8) {
		return atan((this->y + 8 - float(mouseY)) / ((float(mouseX) - this->x - 32)));
	}
	else {
		if (mouseX > this->x - 32) {
			return 0.0001;
		}
		else {
			return -0.00001;
		}
	}
}

int Tank::getA() {
	return this->a;
}

int Tank::getB() {
	return this->b;
}

Projectile* Tank::shoot(int mouseX, int mouseY, int power) {
	return new Projectile(this->getX() + 32 + this->getA(), this->getY() + this->getB() + 8, this->calculateDegree(mouseX, mouseY), power*0.1);
}

void Tank::drawPower(int i){
	if (i < 100){
		al_draw_line(200, 100, 200 + i * 4, 100, al_map_rgb(255, 255, 255), 10);
	}
else
	al_draw_line(200, 100, 600, 100, al_map_rgb(255, 255, 255), 10);
}

bool Tank::isHit(Projectile p) {
	if (&p == NULL) return false;
	if (p.getX() > this->getX() && p.getX() < this->getX() + 64 && p.getY() > this->getY() && p.getY() < this->getY() + 24) {
		return true;
	}
	else return false;
}

void Tank::drawHitbox() {
	al_draw_rectangle(this->getX(), this->getY(), this->getX() + 64, this->getY() + 24, this->color, 4.0);
}

void Enemy::aim() {
	int r = rand() % 100;
}