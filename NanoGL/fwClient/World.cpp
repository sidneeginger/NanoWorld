#include "World.h"
#include <GLFW/glfw3.h>


World::World()
{
}


World::~World()
{
}


int World::Draw()
{
	glPushMatrix();
	
	//auto ftime = glfwGetTime();
	//glRotatef((float)ftime * 50.f, 0.f, 0.f, 1.f);
	glBegin(GL_QUADS);
	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(-100.0f, -100.0f, 0.f);
	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(100.0f, -100.0f, 0.f);
	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(100.0f, 100.0f, 0.f);
	glColor3f(1.f, 0.f, 1.f);
	glVertex3f(-100.0f, 100.0f, 0.f);

	glEnd();

	glPopMatrix();

	return 0;
}
