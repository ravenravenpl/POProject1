#include "terrain.h"
#include <time.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

/*funkcja zwracaj�ca losow� liczb� zmiennoprzecinkow� z zakresu (min, max)
w jakim rozk�adzie to nie wiem, podejrzewam, �e w tym samym, co rand()*/
static float randnum(float min, float max)
{
	int r;
	float x;

	r = rand(); 
	//rand losuje liczby ca�kowite
	//a wi�c losuje si� liczb� z zakresu 0-MAX_RAND, logiczny iloczyn si� robi i dzieli przez MAX_RAND
	//0x7fff to heksadecymalnie zapisane 32 tysi�ce co�tam
	x = (float)(r & 0x7fff) /
		(float)0x7fff;
	//robi si� to po to, by z uzyskanej liczby losowej otrzyma� losowy u�amek z zakresu 0-1
	//nast�pnie zwraca si� ju� standardowo jak przy losowaniu liczb, a wi�c robimy zakres liczb przez odj�cie min od max
	//p�niej mno�ymy go razy nasz losowy wsp�czynnik aby otrzyma� floata i na koniec przesuwamy na osi od zera do minumum (ostatnie + min)
	return (x * (max - min) + min);
}

//zwyk�a funkcja zeruj�ca ca�a tablic� terenu, by by�a gotowa do ponownego u�ytku
void Terrain::clearTable() {
	for (int i = 0; i < MAX_WIDTH+1; i++) {
		this->teren[i] = 0;
	}
}

/*funkcja ustawiaj�ca pierwszy i ostatni element tablicy na losowe warto�ci z przedzia�u (0, 0.8*MAX_HEIGHT)
0.8 wynika z mojego pomys�u na to, by nie generowa� terenu na najwy�szych 20% obszaru gry, by by�o gdzie strzela�
Funkcja istnieje po to, by nast�pne mia�y na czym zacz�� i sko�czy�. Wyg�adzanie polega na wyszukaniu nast�pnej niezerowej
warto�ci w tablicy, a wi�c na start cho�by dwa punkty musz� by� niezerowe by mo�na by�o wyg�adzi� teren*/
void Terrain::setEnds() {
	teren[0] = randnum(0, 0.8*MAX_HEIGHT);
	teren[MAX_WIDTH] = randnum(0, 0.8*MAX_HEIGHT);
}

/*Pojedyncza funkcja wyg�adzaj�ca. Bierze wsp�rz�dne X, Y punktu startowego i ko�cowego
i wype�nia miarowo wszystkie warto�ci tablicy mi�dzy nimi tak, aby stworzy� mi�dzy punktami X, Y prost�*/
void Terrain::smoothElementary(int x1, float y1, int x2, float y2) {
	//obliczenie ile trzeba doda� co krok
	float d = (y2 - y1) / float(x2 - x1);
	for (int i = x1 + 1; i < x2; i++) {
		this->teren[i] = this->teren[i - 1] + d;
	}
}

/*Funkcja wyg�adzaj�ca korzystaj�ca z powy�szej. Przechodzi przez tablic� szukaj�c niezerowej warto�ci
i je�li na tak� natrafi, to wyg�adza teren mi�dzy ostatni� niezerow� i znalezion�.*/
void Terrain::smooth() {
	int ost = 0;
	int x = 1;
	while (x != MAX_WIDTH) {
		if (this->teren[x] != 0) { smoothElementary(ost, teren[ost], x, this->teren[x]); ost = x; }
		x++;
	}
	smoothElementary(ost, this->teren[ost], MAX_WIDTH, this->teren[MAX_WIDTH]);
}

/*Ca�a magia algorytmu. Midpoint displacement algorithm in one dimension.
Algorytm polega na tym, �e bierze si� odcinek, przesuwa si� jego �rodek o losow� warto�� w g�r� lub d�
i powtarza si� dla lewej i prawej po�owy, zmniejszaj�c mo�liwo�� przesuni�cia o wcze�niej ustalon� warto��
opieraj�c� si� na wsp�czynniku. Im mniejszy wsp�czynnik tym mniej mog� si� przesuwa� punkty g�ra/d�, skutkuj�c
w �agodniejszym terenie. Im wi�cej poziom�w rekurencji tym szczeg�owsze tereny powstaj�. IMO 0.8 i 5 daj� fajne
rezulataty pod k�tem gry*/
//offset mo�e by� od 0-1
//level max log2(MAX_WIDTH)
void Terrain::displace(int beg, int end, float offset, int level) {
	//obliczenie �rodka
	int mid = (beg + end) / 2;
	//wyliczenie przesuni�cia, ujemna to w d�, dodatnia to w g�r�
	float r = randnum(-offset*(MAX_HEIGHT/2), offset*(MAX_HEIGHT/2));
	//dodanie do �rodka losowej warto�ci. �rodek jest zerem jak ca�a tablica na pocz�tku, 
	//wi�c trzeba policzy� jego warto�� ze �redniej artymetycznej ko�c�w
	this->teren[mid] = ((this->teren[beg] + this->teren[end]) / 2) + r;
	//je�li zdarzy si�, �e wykroczy si� poza doln� lub g�rn� granic�,
	//to dodaje/odejmuj� si� losow� warto�� tak d�ugo, a� b�dzie git
	while (this->teren[mid] > MAX_HEIGHT*0.8) {
		this->teren[mid] -= randnum(0, 25);
	}
	while (this->teren[mid] < 0) {
		this->teren[mid] += randnum(0, 25);
	}
	//warunek stopu, rekurencja wejdzie tylko na okre�lon� ilo�� poziom�w
	if (level != 0) {
		//powt�rz to wszystko dla lewej i prawej po�owy
		displace(beg, mid, offset*offset, level - 1);
		displace(mid, end, offset*offset, level - 1);
	}
}

/*Prosta funkcja wyr�wnuj�ca teren od beg do end (wyr�wnanie nast�puje do mniejszego z nich)
U�ywana do stawiania czo�g�w, aby sta�y na r�wnym terenie*/
void Terrain::flatten(int beg, int end) {
	//int f = this->teren[end] > this->teren[beg] ? beg : end;
	int f = beg;
	if (beg > end) return; //tutaj wstawi� obs�ug� b��du
	for (int i = beg; i <= end; i++) {
		this->teren[i] = teren[f];
	}
}

//narysowanie terenu polega na narysowaniu MAX_WIDTH pionowych linii, ka�da o wysoko�ci teren[i]
void Terrain::draw() {
	for (int i = 0; i <= MAX_WIDTH; i++) {
		al_draw_line(i, MAX_HEIGHT, i, MAX_HEIGHT - teren[i], al_map_rgb(0, 200, 0), 1.0);
	}
}

float Terrain::getY(int x) {
	return this->teren[x];
}

void Terrain::setY(int x, float y) {
	this->teren[x] = y;
}

void Terrain::reset() {
	Terrain::clearTable();
	Terrain::setEnds();
	Terrain::displace(0, MAX_WIDTH, 0.8, 10);
	Terrain::smooth();
}