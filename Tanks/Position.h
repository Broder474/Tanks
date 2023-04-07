struct Position
{
	Position(int x, int y) : x(x), y(y) {};
	int x, y;
	bool operator==(const Position& pos);
	bool operator!=(const Position& pos);
};