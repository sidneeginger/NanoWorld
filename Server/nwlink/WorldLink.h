#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <thread>
#include "ClientSocket.h"
#include "../Common/WorldPacket.h"
#include "../Common/MessageBuffer.h"
#include "../Common/Timer.h"


class CWorldLink
{
public:
	CWorldLink(boost::asio::io_service& io) 
		: socket_(io), io_service_(io)
	{
		pClient = std::make_shared<ClientSocket>(std::move(socket_));
	}
	~CWorldLink();

public:

private:
	boost::asio::io_service& io_service_;
	boost::asio::ip::tcp::socket socket_;
	std::shared_ptr<ClientSocket> pClient;

public:

	void Init()
	{
		pClient->Connect("127.0.0.1", "5758");
	}
	void SendPlayerPos(float fx, float fy, float fz, float fa)
	{
		MessageBuffer buffer;
		uint16 uCmd = 0x1A;
		uint16 uLen = 0;
		buffer.Write(&uCmd, 2);

		WorldPacket packet;
		packet << fx;
		packet << fy;
		packet << fz;
		packet << fa;

		uLen = packet.size();
		buffer.Write(&uLen, 2);
		buffer.Write(packet.contents(), packet.size());

		auto len = buffer.GetActiveSize();

		pClient->Write(buffer);
	}
};

