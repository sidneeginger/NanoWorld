#pragma once
#include "GLApp.h"
#include "Player.h"
#include "World.h"

class NWApp : public GLApp
{
public:
	NWApp();
	virtual ~NWApp();

private:
	Player m_player;
	World m_world;

protected:
	virtual void Draw();
	virtual void CalcActor();
	virtual void CheckInput();

private:
	void DrawWorld();
};

