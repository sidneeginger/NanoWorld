// fwClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) 

#include "NWApp.h"
#include <iostream>
using namespace std;

int main(void)
{
	NWApp app;
    app.InitResource();
    string sName;
    string sPW;
    cout<< "Input name:" << endl;
    cin >> sName;
    cout<< "Input pw" << endl;
    cin >> sPW;
    app.Login(sName, sPW);
    
    
	app.InitWindows(640, 480);
	app.Run();
}