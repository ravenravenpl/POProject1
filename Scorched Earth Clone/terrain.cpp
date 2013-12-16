#include "terrain.h"
#include <time.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

/*funkcja zwracaj¹ca losow¹ liczbê zmiennoprzecinkow¹ z zakresu (min, max)
w jakim rozk³adzie to nie wiem, podejrzewam, ¿e w tym samym, co rand()*/
static float randnum(float min, float max)
{
	int r;
	float x;

	r = rand(); 
	//rand losuje liczby ca³kowite
	//a wiêc losuje siê liczbê z zakresu 0-MAX_RAND, logiczny iloczyn siê robi i dzieli przez MAX_RAND
	//0x7fff to heksadecymalnie zapisane 32 tysi¹ce coœtam
	x = (float)(r & 0x7fff) /
		(float)0x7fff;
	//robi siê to po to, by z uzyskanej liczby losowej otrzymaæ losowy u³amek z zakresu 0-1
	//nastêpnie zwraca siê ju¿ standardowo jak przy losowaniu liczb, a wiêc robimy zakres liczb przez odjêcie min od max
	//póŸniej mno¿ymy go razy nasz losowy wspó³czynnik aby otrzymaæ floata i na koniec przesuwamy na osi od zera do minumum (ostatnie + min)
	return (x * (max - min) + min);
}

//zwyk³a funkcja zeruj¹ca ca³a tablicê terenu, by by³a gotowa do ponownego u¿ytku
void Terrain::clearTable() {
	for (int i = 0; i < MAX_WIDTH+1; i++) {
		this->teren[i] = 0;
	}
}

/*funkcja ustawiaj¹ca pierwszy i ostatni element tablicy na losowe wartoœci z przedzia³u (0, 0.8*MAX_HEIGHT)
0.8 wynika z mojego pomys³u na to, by nie generowaæ terenu na najwy¿szych 20% obszaru gry, by by³o gdzie strzelaæ
Funkcja istnieje po to, by nastêpne mia³y na czym zacz¹æ i skoñczyæ. Wyg³adzanie polega na wyszukaniu nastêpnej niezerowej
wartoœci w tablicy, a wiêc na start choæby dwa punkty musz¹ byæ niezerowe by mo¿na by³o wyg³adziæ teren*/
void Terrain::setEnds() {
	teren[0] = randnum(0, 0.8*MAX_HEIGHT);
	teren[MAX_WIDTH] = randnum(0, 0.8*MAX_HEIGHT);
}

/*Pojedyncza funkcja wyg³adzaj¹ca. Bierze wspó³rzêdne X, Y punktu startowego i koñcowego
i wype³nia miarowo wszystkie wartoœci tablicy miêdzy nimi tak, aby stworzyæ miêdzy punktami X, Y prost¹*/
void Terrain::smoothElementary(int x1, float y1, int x2, float y2) {
	//obliczenie ile trzeba dodaæ co krok
	float d = (y2 - y1) / float(x2 - x1);
	for (int i = x1 + 1; i < x2; i++) {
		this->teren[i] = this->teren[i - 1] + d;
	}
}

/*Funkcja wyg³adzaj¹ca korzystaj¹ca z powy¿szej. Przechodzi przez tablicê szukaj¹c niezerowej wartoœci
i jeœli na tak¹ natrafi, to wyg³adza teren miêdzy ostatni¹ niezerow¹ i znalezion¹.*/
void Terrain::smooth() {
	int ost = 0;
	int x = 1;
	while (x != MAX_WIDTH) {
		if (this->teren[x] != 0) { smoothElementary(ost, teren[ost], x, this->teren[x]); ost = x; }
		x++;
	}
	smoothElementary(ost, this->teren[ost], MAX_WIDTH, this->teren[MAX_WIDTH]);
}

/*Ca³a magia algorytmu. Midpoint displacement algorithm in one dimension.
Algorytm polega na tym, ¿e bierze siê odcinek, przesuwa siê jego œrodek o losow¹ wartoœæ w górê lub dó³
i powtarza siê dla lewej i prawej po³owy, zmniejszaj¹c mo¿liwoœæ przesuniêcia o wczeœniej ustalon¹ wartoœæ
opieraj¹c¹ siê na wspó³czynniku. Im mniejszy wspó³czynnik tym mniej mog¹ siê przesuwaæ punkty góra/dó³, skutkuj¹c
w ³agodniejszym terenie. Im wiêcej poziomów rekurencji tym szczegó³owsze tereny powstaj¹. IMO 0.8 i 5 daj¹ fajne
rezulataty pod k¹tem gry*/
//offset mo¿e byæ od 0-1
//level max log2(MAX_WIDTH)
void Terrain::displace(int beg, int end, float offset, int level) {
	//obliczenie œrodka
	int mid = (beg + end) / 2;
	//wyliczenie przesuniêcia, ujemna to w dó³, dodatnia to w górê
	float r = randnum(-offset*(MAX_HEIGHT/2), offset*(MAX_HEIGHT/2));
	//dodanie do œrodka losowej wartoœci. Œrodek jest zerem jak ca³a tablica na pocz¹tku, 
	//wiêc trzeba policzyæ jego wartoœæ ze œredniej artymetycznej koñców
	this->teren[mid] = ((this->teren[beg] + this->teren[end]) / 2) + r;
	//jeœli zdarzy siê, ¿e wykroczy siê poza doln¹ lub górn¹ granicê,
	//to dodaje/odejmujê siê losow¹ wartoœæ tak d³ugo, a¿ bêdzie git
	while (this->teren[mid] > MAX_HEIGHT*0.8) {
		this->teren[mid] -= randnum(0, 25);
	}
	while (this->teren[mid] < 0) {
		this->teren[mid] += randnum(0, 25);
	}
	//warunek stopu, rekurencja wejdzie tylko na okreœlon¹ iloœæ poziomów
	if (level != 0) {
		//powtórz to wszystko dla lewej i prawej po³owy
		displace(beg, mid, offset*offset, level - 1);
		displace(mid, end, offset*offset, level - 1);
	}
}

/*Prosta funkcja wyrównuj¹ca teren od beg do end (wyrównanie nastêpuje do mniejszego z nich)
U¿ywana do stawiania czo³gów, aby sta³y na równym terenie*/
void Terrain::flatten(int beg, int end) {
	//int f = this->teren[end] > this->teren[beg] ? beg : end;
	int f = beg;
	if (beg > end) return; //tutaj wstawiæ obs³ugê b³êdu
	for (int i = beg; i <= end; i++) {
		this->teren[i] = teren[f];
	}
}

//narysowanie terenu polega na narysowaniu MAX_WIDTH pionowych linii, ka¿da o wysokoœci teren[i]
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