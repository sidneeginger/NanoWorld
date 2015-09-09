#pragma once
#include "../Common/define.h"

enum PlayerStatus
{
	Player_Unknown,
	Player_Normal,
};

enum  MoveStatus : uint32
{
	MoveNone = 0x0000,
	MoveUp = 0x0001,
	MoveDown = 0x0002,
	MoveTurnLeft = 0x0004,
	MoveTurnRight = 0x0008
};

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

	bool IsMoving() { return m_bIsMoving; };
	uint32 GetMoveStatus() { return m_nMoveStatus; };

private:
	bool m_bIsMoving;
	uint32 m_nMoveStatus;

public:
	void SetMoveStatus(MoveStatus es)
	{
		m_nMoveStatus = m_nMoveStatus | es;
	};

	void CleanMoveStatus(MoveStatus es)
	{
		m_nMoveStatus = m_nMoveStatus & (~es);
	}
};

