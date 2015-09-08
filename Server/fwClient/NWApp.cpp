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

void NWApp::Render()
{
	glClearColor(0.4f, 0.4f, 0.9f, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	//glTranslatef(0, 0, (-100.0f + fTransZ));
	//glRotatef((GLfloat)fEyeAngleX + 90, 1.0f, 0.0f, 0.0f);
	//glRotatef((GLfloat)fEyeAngleZ + 180, 0.0f, 1.0f, 0.0f);
	//if (bTrack)
	//{
	//	glRotatef(-m_player.m_fAngle - 90, 0.0f, 1.0f, 0.0f);
	//}
	gluLookAt(m_player.m_fX, m_player.m_fY, 10, 0, -1000, 0, 0, 0, 1);
	//gluLookAt(player.m_fX, player.m_fY, 10, 0, -1000, 0, 0, 0, 1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	DrawWorld();

	glPopMatrix();
	glFinish();
}


void NWApp::DrawWorld()
{
	glPushMatrix();
	Lighting();

	m_player.Draw(GetDifftime());
	m_world.Draw();

	glPopMatrix();
}
