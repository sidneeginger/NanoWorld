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

int NWApp::Init()
{
	m_pIo = new boost::asio::io_service;
	m_plink = new CWorldLink(*m_pIo);
	m_plink->Init();
	threadPool.push_back(std::thread(boost::bind(&boost::asio::io_service::run, m_pIo)));
		
	return 0;
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

	if (m_player.GetMoveStatus() & MoveUp)
	{
		m_player.MoveUp();
	}
	else if (m_player.GetMoveStatus() & MoveDown)
	{
		m_player.MoveDown();
	}

	if (m_player.GetMoveStatus() & MoveTurnLeft)
	{
		m_player.TurnLeft();
	}
	else if (m_player.GetMoveStatus() & MoveTurnRight)
	{
		m_player.TrunRight();
	}

	if (m_player.GetMoveStatus())
	{
		m_plink->SendPlayerPos(m_player.m_fX, m_player.m_fY, m_player.m_fZ, m_player.m_fAngle);
	}
}


void NWApp::CheckInput()
{
	auto window = GetWindow();
	int state = 0;
	state = glfwGetKey(window, GLFW_KEY_W);
	if (state == GLFW_PRESS)
		m_player.SetMoveStatus(MoveUp);
	else
		m_player.CleanMoveStatus(MoveUp);

	state = glfwGetKey(window, GLFW_KEY_S);
	if (state == GLFW_PRESS)
		m_player.SetMoveStatus(MoveDown);
	else
		m_player.CleanMoveStatus(MoveDown);

	state = glfwGetKey(window, GLFW_KEY_A);
	if (state == GLFW_PRESS)
		m_player.SetMoveStatus(MoveTurnLeft);
	else
		m_player.CleanMoveStatus(MoveTurnLeft);

	state = glfwGetKey(window, GLFW_KEY_D);
	if (state == GLFW_PRESS)
		m_player.SetMoveStatus(MoveTurnRight);
	else
		m_player.CleanMoveStatus(MoveTurnRight);
}

void NWApp::DrawWorld()
{
	glPushMatrix();

	m_player.Draw(GetDifftime());
	m_world.Draw();

	glPopMatrix();
}
