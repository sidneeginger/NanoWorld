#include "NWApp.h"
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment( lib, "glfw3dll.lib")
#pragma comment( lib, "opengl32.lib")
#pragma comment( lib, "glu32.lib")


NWApp::NWApp()
{
}


NWApp::~NWApp()
{
}

void NWApp::Draw()
{
	DrawWorld();
}

void NWApp::CalcActor()
{
	Camera cam = GetCame();
	cam.fCamX = m_player.m_fX;
	cam.fCamY = m_player.m_fY;
	cam.fCamZ = m_player.m_fZ;
	cam.fCamAng = m_player.m_fAngle;
	SetCam(cam);
}


void NWApp::CheckInput()
{
	auto window = GetWindow();
	int state = 0;
	state = glfwGetKey(window, GLFW_KEY_W);
	if (state == GLFW_PRESS)
		m_player.MoveUp();

	state = glfwGetKey(window, GLFW_KEY_S);
	if (state == GLFW_PRESS)
		m_player.MoveDown();

	state = glfwGetKey(window, GLFW_KEY_A);
	if (state == GLFW_PRESS)
		m_player.TurnLeft();

	state = glfwGetKey(window, GLFW_KEY_D);
	if (state == GLFW_PRESS)
		m_player.TrunRight();
}

void NWApp::DrawWorld()
{
	glPushMatrix();

	m_player.Draw(GetDifftime());
	m_world.Draw();

	glPopMatrix();
}
