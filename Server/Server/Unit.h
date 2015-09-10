#pragma once

struct Position
{
	float x;
	float y;
	float z;
	float angle;
};

class Unit
{
public:
	Unit();
	~Unit();

	Position Pos() const { return _pos; }
	void Pos(Position val) { _pos = val; }
private:
	Position _pos;
};

