// fwClient.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) 

#include "NWApp.h"

int main(void)
{
	NWApp app;
	app.InitWindows(640, 480);
	app.Run();
}