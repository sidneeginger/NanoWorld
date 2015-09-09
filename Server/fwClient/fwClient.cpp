// fwClient.cpp : 定义控制台应用程序的入口点。
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