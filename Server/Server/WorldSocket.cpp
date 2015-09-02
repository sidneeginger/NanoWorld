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
	WriteLoginInfo();
}

int WorldSocket::WriteLoginInfo()
{
	MessageBuffer buffer;
	
	char buf[12] = { 0 };
	for (int i = 0; i < 12; i++)
	{
		buf[i] = 'A' + i;
	}

	buffer.Write(buf, 12);

	std::unique_lock<std::mutex> guard(_writeLock);
	QueuePacket(std::move(buffer), guard);

	return 0;
}
