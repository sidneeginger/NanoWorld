#pragma once

struct Position
{
	float x;
	float y;
	float z;
	float angle;

	Position()
		: x(0.0f), y(0.0f), z(0.0f), angle(0.0f)
	{

	}
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

