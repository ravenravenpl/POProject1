#define MAX_WIDTH 800
#define MAX_HEIGHT 600

class Terrain {
protected:
	float teren[MAX_WIDTH + 1];
public:
	void clearTable();
	void setEnds();
	void smoothElementary(int x1, float y1, int x2, float y2);
	void smooth();
	void displace(int beg, int end, float offset, int level);
	void draw();
};