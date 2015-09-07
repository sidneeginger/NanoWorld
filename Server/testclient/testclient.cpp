// testclient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <thread>
#include "../Common/MessageBuffer.h"
#include "../Common/Timer.h"
#include "../Common/WorldPacket.h"

#include "../nwlink/WorldLink.h"

using namespace boost::asio::ip;

class MyClient
{
public:
	MyClient(boost::asio::io_service& io) : socket_(io), io_service_(io)
	{

	}

	void Connect(std::string strIP, std::string strPort)
	{
		tcp::resolver resolver(io_service_);
		tcp::resolver::query query(strIP, strPort);
		tcp::resolver::iterator iterator = resolver.resolve(query);

		boost::asio::async_connect(socket_, iterator,
			boost::bind(&MyClient::handle_connect, this,
				boost::asio::placeholders::error));
	}

	void handle_connect(const boost::system::error_code& error)
	{
		//Send_LoginInfo();

		threadSendPos_ = std::thread(&MyClient::ThreadSend, this);

		if (!error)
		{
			boost::asio::async_read(socket_,
				boost::asio::buffer(recv_buffer_),
				boost::bind(&MyClient::handle_read, this,
					boost::asio::placeholders::error));

			char c = recv_buffer_[0];
			std::cout << c;
		}
		else
		{
			std::cout << "Cannot connected to server. " << error << std::endl;
		}
	}

	void Send_LoginInfo()
	{
		boost::array<char, 4> write_buffer_;
		write_buffer_[0] = 'A';
		write_buffer_[1] = 'E';
		write_buffer_[2] = '8';
		write_buffer_[3] = '6';

		boost::asio::async_write(socket_,
			boost::asio::buffer(write_buffer_.data(),
				write_buffer_.size()),
			boost::bind(&MyClient::handle_write, this,
				boost::asio::placeholders::error));
	}

	void handle_write(const boost::system::error_code& error)
	{

	}

	void handle_read(const boost::system::error_code& error)
	{
		if (!error)
		{
			boost::asio::async_read(socket_,
				boost::asio::buffer(recv_buffer_),
				boost::bind(&MyClient::handle_read, this,
					boost::asio::placeholders::error));

			char c = recv_buffer_[0];
			std::cout << c;

		}
		else
		{
			std::cout << "Cannot connected to server. " << error << std::endl;
		}
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

		boost::asio::async_write(socket_,
			boost::asio::buffer(buffer.GetReadPointer(),
				buffer.GetActiveSize()),
			boost::bind(&MyClient::handle_write, this,
				boost::asio::placeholders::error));
	}

private:
	boost::asio::io_service& io_service_;
	boost::asio::ip::tcp::socket socket_;
	boost::array<char, 1> recv_buffer_;
	std::vector<char> Writebuffer_;
	std::thread threadSendPos_;
	float fposX;
	float fposY;
	float fPosZ;
	float fPosDir;
	uint32 uTime;

public:
	void Run();
	void Init();
};


int main()
{
	boost::asio::io_service io;
	//MyClient client(io);
	//client.Init();
	//client.Run();

	CWorldLink link(io);
	link.Init();
	io.run();


    return 0;
}

void MyClient::Run()
{
	io_service_.run();
}


void MyClient::Init()
{
	Connect("127.0.0.1", "5758");
}
