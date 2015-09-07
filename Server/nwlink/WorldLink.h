#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <thread>
#include "ClientSocket.h"

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
};

