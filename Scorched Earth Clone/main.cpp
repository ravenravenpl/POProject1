#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>

//#include "terrain.h"
//#include "variables.h"
#include "object.h"

#define MAX_WIDTH 800
#define MAX_HEIGHT 600

using namespace std;
const float FPS = 60;

void saveGame();
void loadGame();

Terrain terrain;
Player player;
Enemy enemy;
Projectile *p = NULL;

int main(int argc, char **argv){

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	bool redraw = true;
	bool loaded = false;

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	display = al_create_display(MAX_WIDTH, MAX_HEIGHT);
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize the keyboard!\n");
		return -1;
	}

	if (!al_init_primitives_addon()) {
		fprintf(stderr, "failed to initialize primitives!\n");
		return -1;
	}
	al_install_mouse();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_start_timer(timer);

	srand(time(NULL));

	//int r = 0;
	int mouseX = 0;
	int mouseY = 0;
	int power = 0;
	int wind = 0;
	const float g = 9.8;
	bool mousePressed = false;
	bool playerTurn = true;
	bool shotFired = false;


	while (1)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
			if (p != NULL) {
				p->updateTime(); 
				p->updateGravity();
				p->updateVelocity();
				if (p->detectHit(terrain) || playerTurn && enemy.isHit(*p) || !playerTurn && player.isHit(*p)) {
					if (enemy.isHit(*p) || player.isHit(*p)) {
						printf("Trafiony.");
					}
					delete p;
					p = NULL;
					playerTurn = !playerTurn;
					shotFired = false;
				}
			}
			if (mousePressed) {
				if (power<100) power++;
			}
			//printf("power: %d\n", power);

		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (ev.keyboard.keycode == ALLEGRO_KEY_Q) break;
			if (ev.keyboard.keycode == ALLEGRO_KEY_R)
			{
				terrain.reset();
				player.load(4);
				player.place(&terrain);
				enemy.load(1);
				enemy.place(&terrain);
				loaded = true;
			}
			if (ev.keyboard.keycode == ALLEGRO_KEY_L) {
				loadGame();
			}
			if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
				saveGame();
			}
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
			mouseX = ev.mouse.x;
			mouseY = ev.mouse.y;
			//system("cls");
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
			mousePressed = true;
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			shotFired = true;
			if (playerTurn) {
				p = player.shoot(mouseX, mouseY, power);
			}
			else {
				p = enemy.shoot(mouseX, mouseY, power);
			}
			power = 0;
			mousePressed = false;
		}
		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			al_clear_to_color(al_map_rgb(0, 0, 150));
			if (loaded) {
				terrain.draw();
				player.draw();
				player.drawBarrel();
				//player.drawHitbox();
				enemy.draw();
				enemy.drawBarrel();
				//enemy.drawHitbox();
				if (playerTurn && !shotFired) {
					player.drawPower(power);
					player.updateBarrel(mouseX, mouseY);
				}
				else {
					if (!shotFired) {
						enemy.drawPower(power);
						enemy.updateBarrel(mouseX, mouseY);
					}
				}
			}
			if (p!=NULL) p->draw();
			al_flip_display();
		}
	}

	al_shutdown_primitives_addon();
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}

//nie patrzeæ, tutaj nic nie ma!!
void saveGame() {
	fstream plik;
	plik.open("gra.txt", ios::out);
	for (int i = 0; i <= MAX_WIDTH; i++) {
		plik << terrain.getY(i) << " ";
	}
	plik << endl;
	plik << player.getX() << " " << player.getY() << " " << player.getA() << " "<< player.getB() << " " << endl;
	plik << enemy.getX() << " " << enemy.getY() << " " << enemy.getA() << " " << enemy.getB() << " " << endl;
	plik.close();
}

void loadGame() {
	fstream plik("gra.txt");
	float x, y, a, b;
	for (int i = 0; i <= MAX_WIDTH; i++) {
		plik >> y;
		terrain.setY(i, y);
	}
	plik >> x >> y >> a >> b;
	player.setX(x);
	player.setY(y);
	player.setA(a);
	player.setB(b);
	plik >> x >> y >> a >> b;
	enemy.setX(x);
	enemy.setY(y);
	enemy.setA(a);
	enemy.setB(b);

	plik.close();
}