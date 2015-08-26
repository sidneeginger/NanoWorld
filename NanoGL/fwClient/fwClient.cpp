// fwClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment( lib, "glfw3dll.lib")
#pragma comment( lib, "opengl32.lib")
#pragma comment( lib, "glu32.lib")
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) 

#include "Player.h"
#include "World.h"

float fTransZ = -100.0f;
float fTransX = 0.0f;
float fTransY = 0.0f;
float fEyeAngleX = -60.0f;
float fEyeAngleZ = 0.0f;
int bRClick = 0;
float fMousePosX = 0.0f;
float fMousePosY = 0.0f;

Player player;
World world;


static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void CalcView(GLFWwindow * window)
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float ratio = width / (float)height;

	if (height > 0)
	{
		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, (GLfloat)width / height, 1.0f, 8000.0f);
		glMatrixMode(GL_MODELVIEW);
	}
}

void CheckInput(GLFWwindow * window)
{
	int state = 0;
	state = glfwGetKey(window, GLFW_KEY_W);
	if (state == GLFW_PRESS)
		player.MoveUp();

	state = glfwGetKey(window, GLFW_KEY_S);
	if (state == GLFW_PRESS)
		player.MoveDown();

	state = glfwGetKey(window, GLFW_KEY_A);
	if (state == GLFW_PRESS)
		player.TurnLeft();

	state = glfwGetKey(window, GLFW_KEY_D);
	if (state == GLFW_PRESS)
		player.TrunRight();
}


static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (1 == bRClick)
	{
		fEyeAngleX += (ypos - fMousePosY) * 0.5f;
		

		//if (fEyeAngleX >= 0) fEyeAngleX = 0;
		//if (fEyeAngleX <= -90) fEyeAngleX = -90;

		fEyeAngleZ += (xpos - fMousePosX);
		fMousePosX = xpos;
		fMousePosY = ypos;

		printf("X %f Y %f\n", fEyeAngleX, fEyeAngleZ);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fTransZ += yoffset * 5.5f;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		bRClick = 1;
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		fMousePosX = x;
		fMousePosY = y;
	}
	else
		bRClick = 0;
}

static void win_size(GLFWwindow * window, int nW, int nH)
{
	CalcView(window);
}

void GLRender();

int main(void)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, win_size);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	CalcView(window);


	glClearDepth(1.0f);

	float fTime = 0.0f;
	float fDtime = 0.0f;
	float fNow = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		fNow = glfwGetTime();
		fDtime = fNow - fTime;

		if (fDtime > (1.0f/60.0f) )
		{
			fTime = fNow;
			printf("time %f dTime %f\n", fNow, fDtime);
			GLRender();
			glfwSwapBuffers(window);
			glfwPollEvents();
			CheckInput(window);
		}
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void Lighting(void)
{
	GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat  diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat  specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat  specref[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat	 lightPos[] = { 50.0f, 5.0f, 70.0f, 1.0f };

	glEnable(GL_NORMALIZE);
	//glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate inside of jet
								//glCullFace( GL_BACK );

								// Enable lighting
	glEnable(GL_LIGHTING);

	// Setup and enable light 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// All materials hereafter have full specular reflectivity
	// with a high shine
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 64);
}

void DrawWorld()
{
	glPushMatrix();
	Lighting();

	player.Draw();
	world.Draw();

	glPopMatrix();
}

void GLRender()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();

		glTranslatef(0, 0, (-100.0f + fTransZ));
		glRotatef((GLfloat)fEyeAngleX + 90, 1.0f, 0.0f, 0.0f);
		glRotatef((GLfloat)fEyeAngleZ + 180, 0.0f, 1.0f, 0.0f);	
		//glRotatef(player.m_fAngle*57.29577951f - 90, 0.0f, 1.0f, 0.0f);
		gluLookAt(player.m_fX, player.m_fY, 10, 0, -1000, 0, 0, 0, 1);
		//gluLookAt(player.m_fX, player.m_fY, 10, 0, -1000, 0, 0, 0, 1);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);

		DrawWorld();

	glPopMatrix();
		glFinish();
}