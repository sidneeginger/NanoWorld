// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <gl/gl.h>
#include <gl/glu.h>
#include <mmsystem.h>


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#pragma comment( lib, "winmm.lib")
#pragma comment( lib, "opengl32.lib")	// OpenGL32���ӿ�
#pragma comment( lib, "glu32.lib")		// GLu32���ӿ�
//#pragma comment( lib, "glut32.lib")		// GLu32���ӿ�