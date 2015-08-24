#include "stdafx.h"
#include "GameApp.h"


GameApp::GameApp()
{
}


GameApp::~GameApp()
{
}


void GameApp::Run()
{
	Render();
}


void GameApp::Render()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glFinish();
	SwapBuffers(wglGetCurrentDC());
}


void GameApp::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case 'W': 
		m_player.y += 10.0f;
		break;
	case 'S':
		m_player.y -= 10.0f;
		break;
	case 'A':
		m_player.x -= 10.0f;
		break;
	case 'D':
		m_player.x += 10.0f;
		break;
	}
}


void GameApp::DrawText()
{
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glColor3f(1, 1, 1);

	//glLoadIdentity();
	//glTranslatef(-65, 38, -100);
	//glScalef(0.025, 0.025, 0);

	char string[80] = { 0 };
	char *p = NULL;

	sprintf_s(string, "x:%f y:%f z:%f", m_player.x, m_player.y, m_player.z);

	p = string;
	while (*p)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *p++);
	glPopMatrix();
}
