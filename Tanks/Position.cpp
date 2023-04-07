#include "Position.h"

bool Position::operator==(const Position& pos)
{
	return (this->x == pos.x && this->y == pos.y);
}

bool Position::operator!=(const Position& pos)
{
	return !(this->x == pos.x && this->y == pos.y);
}