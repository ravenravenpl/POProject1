#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <time.h>
#include <stdlib.h>

const float FPS = 60;
const int MAX_WIDTH = 800;
const int MAX_HEIGHT = 600;
const int HALF_HEIGHT = (MAX_HEIGHT / 2) * 0.8;
//int mid;

static float randnum(float min, float max)
{
	int r;
	float	x;

	r = rand();
	x = (float)(r & 0x7fff) /
		(float)0x7fff;
	return (x * (max - min) + min);
}

void smooth_el(float teren[], int x1, float y1, int x2, float y2) {
	//printf("Smoothing terrain between %d and %d:\n", x1, x2);
	float d = (y2-y1) / float(x2-x1);
	//d < 0 ? d = -d : d = d;
	teren[x1] = y1;
	for (int i = x1 + 1; i < x2; i++) {
		teren[i] = teren[i - 1] + d;
		//printf("teren[%d]: %f\n", i, teren[i]);
	}
}

void smooth(float teren[]) {
	int ost = 0;
	int x = 1;
	while (x != MAX_WIDTH) {
		if (teren[x] != 0) { smooth_el(teren, ost, teren[ost], x, teren[x]); ost = x; }
		x++;
	}
	smooth_el(teren, ost, teren[ost], MAX_WIDTH, teren[MAX_WIDTH]);
}

void displace(float tab[], int beg, int end, float offset, int level) {
	//smooth(tab);
	int mid = (beg + end) / 2;
	float r = randnum(-offset*HALF_HEIGHT, offset*HALF_HEIGHT);
	tab[mid] = ((tab[beg] + tab[end]) / 2) + r;
	while (tab[mid] > MAX_HEIGHT*0.8) {
		tab[mid] -= randnum(0, 25);
	}
	while (tab[mid] < 0) {
		tab[mid] += randnum(0, 25);
	}
	//printf("%d: %d %f\n", mid, tab[mid], r
	if (level != 0) {
		//printf("beg: %d, end: %d, mid: %d\n", beg, end, mid);
		displace(tab, beg, mid, offset*offset, level - 1);
		//printf("beg: %d, end: %d, mid: %d\n\n", beg, end, mid);
		displace(tab, mid, end, offset*offset, level - 1);
	}
}

int main(int argc, char **argv){

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	bool redraw = true;

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

	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_flip_display();

	al_start_timer(timer);

	srand(time(NULL));
	int b = 150;
	int skl = 1;
	enum MYKEYS {KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN};
	bool key[4] = { false, false, false, false };
	bool pressed = false;
	float teren[MAX_WIDTH+1];
	float H = 0.8;
	int level = 5;
	int cz1 = 400;
	int cz2 = 400;

	float s = float(MAX_HEIGHT) / float(MAX_WIDTH);

	while (1)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			//b+=skl;
			redraw = true;
			//if (b == 255 || b == 0) skl = 0 - skl;
			if (key[KEY_RIGHT] && H<1.0 && !pressed) { 
				pressed = true;
				//if (key[KEY_CTRL]) {
					//if (b<250) b += 5;
					//else b = 255;
				//} else {
					H += 0.1;
				//}
				system("cls");
				printf("H: %f, Level: %d\n", H, level);
			}
			if (key[KEY_LEFT] && H>0.1 && !pressed) {
				pressed = true;
				//if (key[KEY_CTRL]) {
					//if (b > 5) b -= 5;
					//else b = 0;
				//}
				//else {
					H -= 0.1;
				//}
					system("cls");
				printf("H: %f, Level: %d\n", H, level);
			}
			if (key[KEY_UP] && level < 10 && !pressed) {
				pressed = true;
				level += 1;
				system("cls");
				printf("H: %f, Level: %d\n", H, level);
			}
			if (key[KEY_DOWN] && level > 1 && !pressed) { 
				pressed = true;
				level -= 1;
				system("cls");
				printf("H: %f, Level: %d\n", H, level);
			
			}
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (ev.keyboard.keycode == ALLEGRO_KEY_Q) break;
			if (ev.keyboard.keycode == ALLEGRO_KEY_R) {
				system("cls");
				for (int i = 1; i < MAX_WIDTH; i++) {
					//teren[i] = teren[i-1]+s;
					//printf("%d: %f = %d + %d\n", i, teren[i], teren[i-1], s);
					teren[i] = 0;
				}
				teren[0] = randnum(0, MAX_HEIGHT);
				teren[MAX_WIDTH] = randnum(0, MAX_HEIGHT);
				displace(teren, 0, MAX_WIDTH, H, level);
				for (int i = 0; i <= MAX_WIDTH; i++) {
					if (teren[i]>0) printf("%d ", i);
				}
				smooth(teren);
				cz1 = rand() % 400;
				cz2 = rand() % 400 + 400;
				

			}
			if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
				key[KEY_UP] = true;
			}
			if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
				key[KEY_DOWN] = true;
			}
			if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				key[KEY_RIGHT] = true;
			}
			if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) {
				key[KEY_LEFT] = true;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
				key[KEY_UP] = false;
				pressed = false;
			}
			if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
				key[KEY_DOWN] = false;
				pressed = false;
			}
			if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				key[KEY_RIGHT] = false;
				pressed = false;
			}
			if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) {
				key[KEY_LEFT] = false;
				pressed = false;
			}
		}


		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			al_clear_to_color(al_map_rgb(0, 0, b));
			for (int i = 0; i <= MAX_WIDTH; i++) {
				al_draw_line(i, MAX_HEIGHT, i, MAX_HEIGHT - teren[i], al_map_rgb(0, 200, 0), 1.0);
				al_draw_filled_rectangle(cz1 - 5, MAX_HEIGHT - int(teren[cz1]) - 5, cz1 + 5, MAX_HEIGHT - int(teren[cz1]) + 5, al_map_rgb(150, 150, 0));
				al_draw_filled_rectangle(cz2 - 5, MAX_HEIGHT - int(teren[cz2]) - 5, cz2 + 5, MAX_HEIGHT - int(teren[cz2]) + 5, al_map_rgb(150, 150, 0));
			}
			al_flip_display();
		}
	}

	al_shutdown_primitives_addon();
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}