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

	int selected = 1;
	int selected2 = 10;
	bool mode = true;

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

	al_init_font_addon();
	al_init_ttf_addon();

	ALLEGRO_FONT *font = al_load_ttf_font("lucon.ttf", 16, 0);
	ALLEGRO_FONT *font24 = al_load_font("Helvetica.ttf", 24, 0);
	ALLEGRO_FONT *font36 = al_load_font("Helvetica.ttf", 36, 0);

	if (!font){
		fprintf(stderr, "Could not load 'lucon.ttf'.\n");
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

	//int r = 0;
	int mouseX = 0;
	int mouseY = 0;
	int power = 0;
	int wind = 0;
	const float g = 9.8;
	bool mousePressed = false;
	bool playerTurn = true;
	bool shotFired = false;
	bool gra = false;
	bool menu = false;
	bool menu_poczatek = true;
	bool menu_wybor = false;
	bool poczatek = true;
	bool program = true;

	while (program){
		ALLEGRO_EVENT ev;
		//al_wait_for_event(event_queue, &ev);
		while (menu_poczatek){
			al_wait_for_event(event_queue, &ev);
			if (ev.type == ALLEGRO_EVENT_TIMER){
				redraw = true;
			}
			else if (ev.type == ALLEGRO_EVENT_KEY_DOWN){
				if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
					menu_poczatek = false;
					program = false;
				}
			}
			else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				menu_poczatek = false;
				program = false;
			}
			else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
				mouseX = ev.mouse.x;
				mouseY = ev.mouse.y;
			}
			else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
				if (mouseX > 200 && mouseX<600 && mouseY>200 && mouseY < 300){
					menu_poczatek = false;
					menu_wybor = true;
				}
				if (mouseX >200 && mouseX<600 && mouseY>400 && mouseY < 500){
					selected2 = 2;
					menu_poczatek = false;
					gra = true;
					terrain.reset();
					player.load(selected);
					player.place(&terrain);
					enemy.load(selected2);
					enemy.place(&terrain);
					loaded = true;
					loadGame();

				}
			}
			if (redraw && al_is_event_queue_empty(event_queue)) {
				redraw = false;
				al_clear_to_color(al_map_rgb(0, 0, 150));
				al_draw_filled_rectangle(200, 200, 600, 300, al_map_rgb(100, 100, 100));
				al_draw_filled_rectangle(200, 400, 600, 500, al_map_rgb(100, 100, 100));
				al_draw_text(font36, al_map_rgb(255, 255, 255), 225, 100, 0, "Scorched Earth Clone");
				al_draw_text(font24, al_map_rgb(255, 255, 255), 340, 445, 0, "Wczytaj gre");
				al_draw_text(font24, al_map_rgb(255, 255, 255), 350, 245, 0, "Nowa gra");
				al_flip_display();
				//cout << mouseX << endl;
				//cout << mouseY << endl;
			}
		}
		while (menu_wybor){
			al_wait_for_event(event_queue, &ev);
			if (ev.type == ALLEGRO_EVENT_TIMER){
				redraw = true;
			}
			else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				menu_wybor = false;
				program = false;
			}
			else if (ev.type == ALLEGRO_EVENT_KEY_DOWN){
				if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
					menu_poczatek = true;
					menu_wybor = false;
				}
			}
			else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
				mouseX = ev.mouse.x;
				mouseY = ev.mouse.y;
			}
			else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
				if (mouseX > 300 && mouseX<500 && mouseY>200 && mouseY < 300){
					menu_wybor = false;
					menu = true;
					mode = true;
					selected2 = 2;
				}
				if (mouseX >200 && mouseX<600 && mouseY>400 && mouseY < 500){
					menu_wybor = false;
					menu = true;
					mode = false;
					selected2 = 10;
				}
			}
			if (redraw && al_is_event_queue_empty(event_queue)) {
				redraw = false;
				al_clear_to_color(al_map_rgb(0, 0, 150));
				al_draw_filled_rectangle(200, 200, 600, 300, al_map_rgb(100, 100, 100));
				al_draw_filled_rectangle(200, 400, 600, 500, al_map_rgb(100, 100, 100));
				al_draw_text(font36, al_map_rgb(255, 255, 255), 225, 100, 0, "Scorched Earth Clone");
				al_draw_text(font24, al_map_rgb(255, 255, 255), 300, 445, 0, "Graj z komputerem");
				al_draw_text(font24, al_map_rgb(255, 255, 255), 325, 245, 0, "Dwoch graczy");
				al_flip_display();
				//cout << mouseX << endl;
				//cout << mouseY << endl;
			}
		}
		while (menu){
			al_wait_for_event(event_queue, &ev);
			if (ev.type == ALLEGRO_EVENT_TIMER){
				redraw = true;
			}
			else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				menu = false;
				program = false;
			}
			else if (ev.type == ALLEGRO_EVENT_KEY_DOWN){
				if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT){
					if (selected == 9){
						selected = 1;
					}
					else{
						selected = selected + 1;
					}
				}
				if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT){
					if (selected == 1){
						selected = 9;
					}
					else{
						selected = selected - 1;
					}
				}
				if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
					menu = false;
					menu_wybor = true;
				}
				if (mode){
					if (ev.keyboard.keycode == ALLEGRO_KEY_UP){
						if (selected2 == 9){
							selected2 = 1;
						}
						else{
							selected2 = selected2 + 1;
						}
					}
					if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN){
						if (selected2 == 1){
							selected2 = 9;
						}
						else{
							selected2 = selected2 - 1;
						}
					}
				}
			}
			else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
				mouseX = ev.mouse.x;
				mouseY = ev.mouse.y;
			}

			else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
				if (mouseX > 100 && mouseX<200 && mouseY>100 && mouseY < 200){
					if (selected == 1){
						selected = 9;
					}
					else{
						selected--;
					}
				}
				if (mouseX >600 && mouseX<700 && mouseY>100 && mouseY < 200){
					if (selected == 9){
						selected = 1;
					}
					else{
						selected++;
					}
				}
				if (mouseX > 100 && mouseX<200 && mouseY>300 && mouseY < 400 && mode){
					if (selected2 == 1){
						selected2 = 9;
					}
					else{
						selected2--;
					}
				}
				if (mouseX >600 && mouseX<700 && mouseY>300 && mouseY < 400 && mode){
					if (selected2 == 9){
						selected2 = 1;
					}
					else{
						selected2++;
					}
				}
				if (mouseX >200 && mouseX<600 && mouseY>450 && mouseY < 550){
					menu = false;
					gra = true;
					terrain.reset();
					player.load(selected);
					player.place(&terrain);
					enemy.load(selected2);
					enemy.place(&terrain);
					loaded = true;
				}
			}
			if (redraw && al_is_event_queue_empty(event_queue)) {
				redraw = false;
				al_clear_to_color(al_map_rgb(0, 0, 150));

				al_draw_filled_triangle(600, 100, 600, 200, 700, 150, al_map_rgb(100, 100, 100));
				al_draw_filled_triangle(100, 150, 200, 200, 200, 100, al_map_rgb(100, 100, 100));
				al_draw_text(font24, al_map_rgb(255, 255, 255), 350, 50, 0, "Gracz 1");
				if (mode){
					al_draw_filled_triangle(600, 400, 600, 300, 700, 350, al_map_rgb(100, 100, 100));
					al_draw_filled_triangle(100, 350, 200, 300, 200, 400, al_map_rgb(100, 100, 100));
					al_draw_text(font24, al_map_rgb(255, 255, 255), 350, 250, 0, "Gracz 2");
					enemy.load(selected2);
					enemy.drawMenu(350, 350);
				}

				al_draw_filled_rectangle(200, 450, 600, 550, al_map_rgb(100, 100, 100));
				al_draw_text(font24, al_map_rgb(255, 255, 255), 322, 490, 0, "Rozpocznij gre");
				player.load(selected);
				player.drawMenu(350, 150);
				al_flip_display();
				//cout << mouseX << endl;
				//cout << mouseY << endl;
			}

		}
		while (gra)
		{
			//ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);

			if (ev.type == ALLEGRO_EVENT_TIMER) {
				redraw = true;
				if (p != NULL) {
					p->updateTime();
					p->updateY();
					p->updateX();
					p->updateVelocity(wind);
					if (p->detectHit(terrain) || playerTurn && enemy.isHit(*p) || !playerTurn && player.isHit(*p)) {
						if (enemy.isHit(*p) || player.isHit(*p)) {
							//printf("trafiony.");
							gra = false;
							menu_poczatek = true;
							break;
						}
						delete p;
						p = NULL;
						playerTurn = !playerTurn;
						shotFired = false;
					}
				}
				if (mousePressed) {
					if (power < 100) power++;
				}
				if (mode == false){
					if (!playerTurn && !shotFired) {
						wind = rand() % 20 - 10;
						shotFired = true;
						p = enemy.aim();
					}
				}
				//printf("power: %d\n", power);

			}
			else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				gra = false;
				program = false;
			}
			else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
				if (ev.keyboard.keycode == ALLEGRO_KEY_R)
				{
					terrain.reset();
					player.load(selected);
					player.place(&terrain);
					enemy.load(selected2);
					enemy.place(&terrain);
					loaded = true;
				}
				if (ev.keyboard.keycode == ALLEGRO_KEY_L) {
					loadGame();
				}
				if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
					saveGame();
				}
				if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
					gra = false;
					menu_poczatek = true;
				}
			}
			else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
				mouseX = ev.mouse.x;
				mouseY = ev.mouse.y;
				//system("cls");
			}
			else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
				mousePressed = true;
				poczatek = false;
			}
			else
			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && poczatek == false) {
				wind = rand() % 20 - 10;
				shotFired = true;
				if (playerTurn) {
					p = player.shoot(mouseX, mouseY, power);
				}
				else {
					if (mode == false){
						int r = rand() % 100;
						int x = rand() % 100;
						int y = rand() % 100;
						p = enemy.shoot(enemy.getX() - x, enemy.getY() - y, r);
					}
					else{
						p = enemy.shoot(mouseX, mouseY, power);
					}
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
				if (p != NULL) p->draw();
				al_draw_textf(font, al_map_rgb(255, 255, 255), 50, 50, ALLEGRO_ALIGN_LEFT, "wind: %d", wind);
				al_draw_textf(font, al_map_rgb(255, 255, 255), 50, 70, ALLEGRO_ALIGN_LEFT, "power: %d", power);
				al_flip_display();
			}
		}
		poczatek = true;
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
	plik << player.getX() << " " << player.getY() << " " << player.getA() << " " << player.getB() << " " << endl;
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