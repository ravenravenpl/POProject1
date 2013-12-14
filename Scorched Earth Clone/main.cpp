#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5\allegro_image.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>

#include "terrain.h"
//#include "variables.h"
#include "object.h"

#define MAX_WIDTH 800
#define MAX_HEIGHT 600

using namespace std;
const float FPS = 60;

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

	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_register_event_source(event_queue, al_get_mouse_event_source());

	al_start_timer(timer);

	srand(time(NULL));


	Terrain terrain;
	Tank tank;
	Projectile *p = NULL;

	int r = 0;
	int mouseX = 0;
	int mouseY = 0;
	const float g = 9.8;


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
			}
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (ev.keyboard.keycode == ALLEGRO_KEY_Q) break;
			if (ev.keyboard.keycode == ALLEGRO_KEY_R)
			{
				terrain.reset();
				r = rand() % MAX_WIDTH - 70;
				tank.load(4);
				tank.place(r, MAX_HEIGHT - terrain.getY(r));
				terrain.flatten(r, r + 64);
				loaded = true;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
			mouseX = ev.mouse.x;
			mouseY = ev.mouse.y;
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
			p = new Projectile(tank.getX(), tank.getY());
		}
		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			al_clear_to_color(al_map_rgb(0, 0, 150));
			if (loaded) {
				terrain.draw();
				tank.draw();
				tank.updateBarrel(mouseX, mouseY);
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