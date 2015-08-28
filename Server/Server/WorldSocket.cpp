#include "stdafx.h"
#include "WorldSocket.h"


WorldSocket::WorldSocket(tcp::socket&& socket) 
	: Socket(std::move(socket))
{
}


WorldSocket::~WorldSocket()
{
}

void WorldSocket::Start()
{

}

void WorldSocket::ReadHandler()
{

}