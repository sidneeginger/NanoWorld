#include "stdafx.h"
#include "GLApp.h"
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment( lib, "glfw3dll.lib")
#pragma comment( lib, "opengl32.lib")
#pragma comment( lib, "glu32.lib")
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) 


bool GLApp::bTrack = false;
float GLApp::fTransZ = 0.0f;
float GLApp::fTransX = 0.0f;
float GLApp::fTransY = 0.0f;
 float GLApp::fEyeAngleX = -60.0f;
 float GLApp::fEyeAngleZ = 0.0f;
 int GLApp::bRClick = 0;
 float GLApp::fMousePosX = 0.0f;
 float GLApp::fMousePosY = 0.0f;

GLApp::GLApp()
	: m_nRefreshRate(60)
{

}


GLApp::~GLApp()
{

}


int GLApp::InitWindows(int nWidth, int nHeight)
{
	glfwSetErrorCallback(&GLApp::error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	m_pWindow = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!m_pWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(m_pWindow);
	glfwSwapInterval(1);
	glfwSetKeyCallback(m_pWindow, &GLApp::key_callback);
	glfwSetWindowSizeCallback(m_pWindow, &GLApp::win_size);
	glfwSetCursorPosCallback(m_pWindow, cursor_pos_callback);
	glfwSetMouseButtonCallback(m_pWindow, mouse_button_callback);
	glfwSetScrollCallback(m_pWindow, scroll_callback);
	CalcView(m_pWindow);

	glClearDepth(1.0f);

	Init();

	return 0;
}

void GLApp::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fTransZ += yoffset * 5.5f;
}

void GLApp::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
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


int GLApp::Run()
{
	float fTime = glfwGetTime();
	float fNow = 0.0f;
	float fRateTime = 1.0f / (float)m_nRefreshRate;

	while (!glfwWindowShouldClose(m_pWindow))
	{
		fNow = glfwGetTime();
		m_fDTime = fNow - fTime;
		if (m_fDTime >= fRateTime)
		{
			fTime = fNow;
			CheckInput();
			CalcActor();
			Render();
			glfwSwapBuffers(m_pWindow);
			glfwPollEvents();
		}
	
	}

	Finish();
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}

void GLApp::error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void GLApp::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	else if (key == GLFW_KEY_C && action == GLFW_PRESS)
		bTrack = !bTrack;
}

void GLApp::win_size(GLFWwindow * window, int nW, int nH)
{
	CalcView(window);
}

void GLApp::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
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

void GLApp::CalcView(GLFWwindow * window)
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


void GLApp::Render()
{
	glClearColor(0.4f, 0.4f, 0.9f, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glTranslatef(0, 0, (-100.0f + fTransZ));
	glRotatef((GLfloat)fEyeAngleX + 90, 1.0f, 0.0f, 0.0f);
	//glRotatef((GLfloat)fEyeAngleZ + 180, 0.0f, 1.0f, 0.0f);
	if (bTrack)
	{
		glRotatef(-m_cam.fCamAng - 90, 0.0f, 1.0f, 0.0f);
	}
	gluLookAt(m_cam.fCamX, m_cam.fCamY, 10, 0, -1000, 0, 0, 0, 1);
	//gluLookAt(player.m_fX, player.m_fY, 10, 0, -1000, 0, 0, 0, 1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	Lighting();

	Draw();

	glPopMatrix();
	glFinish();
}


void GLApp::CheckInput()
{

}


void GLApp::Lighting()
{
	GLfloat  ambientLight[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat  diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat  specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat  specref[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat	 lightPos[] = { 150.0f, 5.0f, 170.0f, 1.0f };

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


void GLApp::Draw()
{

}


void GLApp::CalcActor()
{
}


int GLApp::Init()
{
	return 0;
}

int GLApp::Finish()
{
	return 0;
}
