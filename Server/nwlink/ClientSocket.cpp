#include "stdafx.h"
#include "ClientSocket.h"
#include "WorldLink.h"


void ClientSocket::ConnectHandler()
{
	_pworldLink->SendLoginInfo();
}

void ClientSocket::ReadHandler()
{

}