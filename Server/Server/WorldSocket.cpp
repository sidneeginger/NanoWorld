#include "stdafx.h"
#include "WorldSocket.h"
#include <iostream>
#include "bitstream.h"


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

	AsyncRead();
}

void WorldSocket::ReadHandler()
{
	BitStream stream(std::move(GetReadBuffer()));

	while (!stream.IsRead())
	{
		uint8 c1 = stream.Read<uint8>(8);

		std::cout << c1;
	}

}