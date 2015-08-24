#pragma once
#include "Player.h"

class GameApp
{
public:
	GameApp();
	~GameApp();
	void Run();
	void Render();
	void OnKeyDown(WPARAM wParam, LPARAM lParam);

	Player m_player;
	void DrawText();
};

