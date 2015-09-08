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
	void Render();
public:
	void DrawWorld();
};

