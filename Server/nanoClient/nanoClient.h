
// nanoClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CnanoClientApp: 
// �йش����ʵ�֣������ nanoClient.cpp
//

class CnanoClientApp : public CWinApp
{
public:
	CnanoClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CnanoClientApp theApp;