#pragma once
class Player
{
public:
	Player();
	~Player();
	void Draw(float fTime);

	float m_fX;
	float m_fY;
	float m_fZ;
	float m_fAngle;
	float m_fVec;
	float m_fMove;
	double m_fTime;
	double m_dTime;

	void MoveUp();
	void MoveDown();
	void TurnLeft();
	void TrunRight();
	void StopMove();
};

