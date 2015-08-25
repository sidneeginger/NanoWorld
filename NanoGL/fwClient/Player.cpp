#include "Player.h"
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

Player::Player()
{
	m_fX = 0.0f;
	m_fY = 0.0f;
}


Player::~Player()
{
}


void Player::Draw()
{
	glPushMatrix();

	glColor3f(0.65f, 0.65f, 0.65f);
	glTranslatef(m_fX, m_fY, 3.0f);
	GLUquadricObj *qobj = gluNewQuadric();
	gluSphere(qobj, 3, 16, 16);

	//gluCylinder(qobj, 13, 8, 16, 16, 16);

	//gluDisk(qobj, 5, 25, 20, 4);

	glPopMatrix();
}
