#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include "object.h"

//rozmiary okna z gr¹
#define MAX_WIDTH 800
#define MAX_HEIGHT 600

//iloœæ klatek na sekundê
using namespace std;
const float FPS = 60;

void saveGame();
void loadGame();

Terrain terrain;
Player player;
Enemy enemy;
Projectile *p = NULL;
bool mode = true;

int main(int argc, char **argv){

	//inicjalizowanie zmiennych i potrzebnych funkcji z Allegro
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	bool redraw = true;
	bool loaded = false;

	int selected = 1;
	int selected2 = 10;

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

	//g³ówna pêtla programu
	while (program){
		ALLEGRO_EVENT ev;
		//czêœæ programu wyœwietlaj¹ca pocz¹tkowe menu
		while (menu_poczatek){
			al_wait_for_event(event_queue, &ev);
			//odœwie¿anie ekranu w ka¿dej klatce animacji
			if (ev.type == ALLEGRO_EVENT_TIMER){
				redraw = true;
			}
			//wyjœcie z programu przez zmianê wartoœci w pêtlach programu i menu na false
			else if (ev.type == ALLEGRO_EVENT_KEY_DOWN){
				if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
					menu_poczatek = false;
					program = false;
				}
			}
			//reakcja na naciœniêcie krzy¿yka w prawym górnym rogu
			else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				menu_poczatek = false;
				program = false;
			}
			//zapisywanie bie¿¹cych wspó³rzêdnych myszki
			else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
				mouseX = ev.mouse.x;
				mouseY = ev.mouse.y;
			}
			//w przypadku naciœniêcia przycisku myszy sprawdzana jest pozycja kursora
			//i zale¿nie od tego, nad którym przyciskiem by³, podejmowana jest odpowiednia
			//akcja
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
			//rysowanie menu
			if (redraw && al_is_event_queue_empty(event_queue)) {
				redraw = false;
				al_clear_to_color(al_map_rgb(0, 0, 150));
				al_draw_filled_rectangle(200, 200, 600, 300, al_map_rgb(100, 100, 100));
				al_draw_filled_rectangle(200, 400, 600, 500, al_map_rgb(100, 100, 100));
				al_draw_text(font36, al_map_rgb(255, 255, 255), 225, 100, 0, "Scorched Earth Clone");
				al_draw_text(font24, al_map_rgb(255, 255, 255), 340, 445, 0, "Wczytaj gre");
				al_draw_text(font24, al_map_rgb(255, 255, 255), 350, 245, 0, "Nowa gra");
				al_flip_display();
			}
		}
		//ekran wyboru trybu gry
		while (menu_wybor){
			al_wait_for_event(event_queue, &ev);
			//eventy jak wy¿ej
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
			}
		}
		//menu wyboru czo³gu
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
			}

		}
		//w³aœciwa pêtla gry
		while (gra)
		{
			al_wait_for_event(event_queue, &ev);

			if (ev.type == ALLEGRO_EVENT_TIMER) {
				redraw = true;
				//jeœli pocisk istnieje, to przy ka¿dej klatce uaktualniana jest jego pozycja
				//oraz prêdkoœæ pozioma zgodnie z wiej¹cym wiatrem
				if (p != NULL) {
					p->updateTime();
					p->updateY();
					p->updateX();
					p->updateVelocity(wind);
					//w przypadku trafienia gra jest koñczona i wraca do menu
					if (p->detectHit(terrain) || playerTurn && enemy.isHit(*p) || !playerTurn && player.isHit(*p)) {
						if (enemy.isHit(*p) || player.isHit(*p)) {
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
				//zwiêkszanie mocy w miarê trzymania przycisku myszy
				if (mousePressed) {
					if (power < 100) power++;
				}
				//jeœli gramy z przeciwnikiem, to tutaj nastêpuje wywo³anie metody celowania
				if (mode == false){
					if (!playerTurn && !shotFired) {
						wind = rand() % 20 - 10;
						shotFired = true;
						p = enemy.aim();
					}
				}
			}
			else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				gra = false;
				program = false;
			}
			else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
				//reakcja na przycisk R, który pozwala prze³adowaæ plansze i czo³gi
				if (ev.keyboard.keycode == ALLEGRO_KEY_R)
				{
					terrain.reset();
					player.load(selected);
					player.place(&terrain);
					enemy.load(selected2);
					enemy.place(&terrain);
					loaded = true;
					p = NULL;
					shotFired = false;
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
			}
			else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
				mousePressed = true;
				poczatek = false;
			}
			else
			//po puszczeniu przycisku myszy nastêpuje strza³
			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && poczatek == false) {
				wind = rand() % 20 - 10;
				shotFired = true;
				//zale¿nie od tego, czy strzela przeciwnik czy gracz obiekt projectile tworzy
				//czo³g gracza lub przeciwnika
				if (playerTurn) {
					p = player.shoot(mouseX, mouseY, power);
				}
				else {
					if (mode == false){
						//strza³em zajmuje siê "sztuczna inteligencja"
						p = enemy.aim();
					}
					else{
						p = enemy.shoot(mouseX, mouseY, power);
					}
				}
				power = 0;
				mousePressed = false;
			}
			//tutaj nastêpuje rysowanie terenu, czo³gów oraz pocisku
			if (redraw && al_is_event_queue_empty(event_queue)) {
				redraw = false;
				al_clear_to_color(al_map_rgb(0, 0, 150));
				if (loaded) {
					terrain.draw();
					player.draw();
					player.drawBarrel();
					enemy.draw();
					enemy.drawBarrel();
					//jeœli jest ruch gracza i nie jest on podczas strza³u, to lufa jego czo³gu
					//pod¹¿a za kursorem
					if (playerTurn && !shotFired) {
						player.drawPower(power);
						player.updateBarrel(mouseX, mouseY);
					}
					else {
					//to samo, ale dla przeciwnika
						if (!shotFired) {
							enemy.drawPower(power);
							enemy.updateBarrel(mouseX, mouseY);
						}
					}
				}
				if (p != NULL) p->draw();
				al_draw_textf(font, al_map_rgb(255, 255, 255), 50, 50, ALLEGRO_ALIGN_LEFT, "wiatr: %d", wind);
				al_draw_textf(font, al_map_rgb(255, 255, 255), 50, 70, ALLEGRO_ALIGN_LEFT, "moc: %d", power);
				al_flip_display();
			}
		}
		poczatek = true;
	}

	//deinicjacja funkcji allegro
	al_shutdown_primitives_addon();
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}

//funkcje zapisu i odczytu gry
void saveGame() {
	fstream plik;
	plik.open("gra.txt", ios::out);
	for (int i = 0; i <= MAX_WIDTH; i++) {
		plik << terrain.getY(i) << " ";
	}
	plik << endl;
	plik << player.getX() << " " << player.getY() << " " << player.getA() << " " << player.getB() << " " << endl;
	plik << player.getI() << endl;
	plik << enemy.getX() << " " << enemy.getY() << " " << enemy.getA() << " " << enemy.getB() << " " << endl;
	plik << enemy.getI() << endl;
	plik << (mode ? "1" : "0") << endl;
	plik.close();
}

void loadGame() {
	fstream plik("gra.txt");
	float x, y, a, b, i;
	for (int i = 0; i <= MAX_WIDTH; i++) {
		plik >> y;
		terrain.setY(i, y);
	}
	plik >> x >> y >> a >> b;
	plik >> i;
	player.setX(x);
	player.setY(y);
	player.setA(a);
	player.setB(b);
	player.load(i);
	plik >> x >> y >> a >> b;
	plik >> i;
	enemy.setX(x);
	enemy.setY(y);
	enemy.setA(a);
	enemy.setB(b);
	enemy.load(i);
	plik >> i;
	mode = i ? TRUE : FALSE;
	plik.close();
}