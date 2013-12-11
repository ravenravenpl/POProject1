class Object {
protected:
	float x, y;
public:
	float getX();
	float getY();
};

class Projectile {
protected:
	float vertVelocity, horVelocity;
public:
	void updateVelocity();
	void move();
	bool detectHit();
};

class Tank {
protected:
	int HP, degree, power;
public:
	Projectile shoot();
	void damage();
	void getPos();
};