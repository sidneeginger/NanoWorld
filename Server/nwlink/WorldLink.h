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

		fposX = 0.0f;
		fposY = 0.0f;
	}
	~CWorldLink();

public:

private:
	boost::asio::io_service& io_service_;
	boost::asio::ip::tcp::socket socket_;
	std::shared_ptr<ClientSocket> pClient;
	float fposX;
	float fposY;
	std::thread threadSendPos_;
	uint32 uTime;
public:

	void Init()
	{
		pClient->Connect("127.0.0.1", "5758");
		if(pClient->IsOpen())
			threadSendPos_ = std::thread(&CWorldLink::ThreadSend, this);
	}

	void ThreadSend()
	{
		while (1)
		{
			auto nTime = getMSTime();
			if (nTime - uTime > 200)
			{
				uTime = nTime;
				SendPos();
			}
		}
	}

	void SendPos()
	{
		fposX += 9.2f;
		fposY += 2.3f;

		MessageBuffer buffer;
		uint16 uCmd = 0x1A;
		uint16 uLen = 8;
		buffer.Write(&uCmd, 2);
		buffer.Write(&uLen, 2);


		WorldPacket packet;
		packet << fposX;
		packet << fposY;
		buffer.Write(packet.contents(), packet.size());

		auto len = buffer.GetActiveSize();

		pClient->Write(buffer);
	}
};

