#pragma once

class Tank
{
public:
	Tank(int x, int y);
	~Tank();
	int getX() { return x; }
	void setX(int x) { this->x = x; }
	int getY() { return y; }
	void setY(int y) { this->y = y; }
private:
	int x = 0, y = 0;
};