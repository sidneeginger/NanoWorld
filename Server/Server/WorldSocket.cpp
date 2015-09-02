#include "stdafx.h"
#include "WorldSocket.h"
#include <iostream>


WorldSocket::WorldSocket(tcp::socket&& socket) 
	: Socket(std::move(socket))
{
}


WorldSocket::~WorldSocket()
{
}

void WorldSocket::Start()
{
	std::string ip_address = GetRemoteIpAddress().to_string();
	auto nPort = GetRemotePort();
	std::cout << ip_address << ":" << nPort << std::endl;
}

void WorldSocket::ReadHandler()
{

}