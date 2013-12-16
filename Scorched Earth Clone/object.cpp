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

void Object::setX(float x) {
	this->x = x;
}

float Object::getY() {
	return this->y;
}

void Object::setY(float y) {
	this->y = y;
}

Projectile::Projectile(float x, float y, float degree, float power) {
	this->x = x;
	this->y = y;
	this->y0 = y;
	this->t = 1;
	this->power = power;
	//inicjalizowanie prêdkoœci poziomiej i pionowej zgodnie z prawami fizyki
	this->horVelocity = sin(degree) <= 0 ? -cos(degree)*power : cos(degree)*power;
	this->vertVelocity = abs(sin(degree))*power;
}

void Projectile::updateTime() {
	this->t++;
}

//wzór na rzut pionowy
void Projectile::updateY() {
	this->y = this->y0 - (vertVelocity*t - (0.5*G*t*t)*0.01);
}

//w poziomie ruch jednostajny
void Projectile::updateX() {
	this->x = this->x + this->horVelocity;
	//this->y = this->y + this->vertVelocity;
}

//w przypadku wiatru prêdkoœæ pozioma jest modyfikowana o wartoœæ wiatru
void Projectile::updateVelocity(int wind) {
	this->horVelocity = this->horVelocity + 0.001*wind;
}

//wykrycie zderzenia z terenem, jeœli pocisk jest ni¿ej ni¿ ziemia
//to znaczy ¿e trafi³
bool Projectile::detectHit(Terrain terrain) {
	if (this->y + 4 > MAX_HEIGHT - terrain.getY(this->x)) {
		return true;
	}
	else return false;
}

void Projectile::draw() {
	al_draw_filled_rectangle((this->x) + 2, (this->y) + 2, (this->x) - 2, (this->y) - 2, al_map_rgb(150, 0, 0));
}

//obliczenie k¹ta jaki tworzy lufa z czo³giem
//wykorzystanie arcusa tangensa do tego celu
float Tank::calculateDegree(int mouseX, int mouseY){
	//jeœli mysz jest powy¿ej czo³gu
	if (mouseY < this->y + 8) {
		return atan((this->y + 8 - float(mouseY)) / ((float(mouseX) - this->x - 32)));
	}
	//jeœli nie, to zwracana jest wartoœæ zerowa (lufa nie opada ni¿ej)
	//minimalna wartoœæ po to, by za pomoc¹ znaku ustaliæ kierunek lufy (lewo czy prawo)
	else {
		if (mouseX > this->x - 32) {
			return 0.00001;
		}
		else {
			return -0.00001;
		}
	}
}

int Tank::getA() {
	return this->a;
}

void Tank::setA(int a) {
	this->a = a;
}

int Tank::getB() {
	return this->b;
}

void Tank::setB(int a) {
	this->b = b;
}

int Tank::getI() {
	return this->i;
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
	this->i = i;
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
	case 5:
		image = al_load_bitmap("czerwony_1.png");
		this->color = al_map_rgb(197, 96, 96);
		break;
	case 6:
		image = al_load_bitmap("zielony_1.png");
		this->color = al_map_rgb(97, 197, 156);
		break;
	case 7:
		image = al_load_bitmap("niebieski_1.png");
		this->color = al_map_rgb(88, 153, 227);
		break;
	case 8:
		image = al_load_bitmap("fioletowy_1.png");
		this->color = al_map_rgb(227, 89, 225);
		break;
	case 9:
		image = al_load_bitmap("zielony_2.png");
		this->color = al_map_rgb(27, 155, 30);
		break;
	case 10:
		image = al_load_bitmap("komp.png");
		this->color = al_map_rgb(0, 0, 0);
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

//strza³ zwraca obiekt typu Projectile z zainicjalizowanymi podanymi parametrami
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

//proste sprawdzanie, czy pocisk trafi³ porównuj¹c wspó³rzêdne pocisku i czo³gu
//jeœli pocisk znalaz³ siê w obrêbie czo³gu, to znaczy, ¿e nast¹pi³o trafienie
bool Tank::isHit(Projectile p) {
	if (&p == NULL) return false;
	if (p.getX() > this->getX() && p.getX() < this->getX() + 64 && p.getY() > this->getY() && p.getY() < this->getY() + 24) {
		return true;
	}
	else return false;
}

//u¿ywane do celów debugowania
void Tank::drawHitbox() {
	al_draw_rectangle(this->getX(), this->getY(), this->getX() + 64, this->getY() + 24, this->color, 4.0);
}

void Tank::drawMenu(int x, int y){
	al_draw_bitmap(image, x, y, 0);
}

//inicjalizacja kierunku luf przeciwnika i gracza w odpowiednim kierunku
Player::Player() {
	this->a = 39;
	this->b = 0;
}

//postawienie gracza w losowym miejscu lewej po³owy mapy
void Player::place(Terrain* terrain) {
	int r = rand() % (MAX_WIDTH / 2 - 50) + 50;
	this->x = r;
	this->y = MAX_HEIGHT - terrain->getY(r) - 24;
	terrain->flatten(r, r + 64);
}

Enemy::Enemy() {
	this->a = -39;
	this->b = 0;
}

//analogiczna metoda jak dla gracza, ale czo³g ustawiany jest w prawej po³owie ekranu
void Enemy::place(Terrain* terrain) {
	int r = rand() % (MAX_WIDTH / 2 - 50) + 350;
	this->x = r;
	this->y = MAX_HEIGHT - terrain->getY(r) - 24;
	terrain->flatten(r, r + 64);
}


//przeciwnik celuje przez losowe wybranie punktu z zakresu
//100 pikseli w lewo i 100 pikselu do góry, by w ten sposób
//wytworzyæ losowe nachylenie lufy oraz z losow¹ si³¹
Projectile* Enemy::aim() {
	int r = rand() % 100;
	int x = rand() % 100;
	int y = rand() % 100;
	this->updateBarrel(this->x - x, this->y - y);
	return this->shoot(this->x - x, this->y - y, r);
}

