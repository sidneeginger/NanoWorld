// testclient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

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
		Send_LoginInfo();

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

private:
	boost::asio::io_service& io_service_;
	boost::asio::ip::tcp::socket socket_;
	boost::array<char, 1> recv_buffer_;
};


int main()
{
	boost::asio::io_service io_service;
	
	MyClient client(io_service);
	client.Connect("127.0.0.1", "5758");

	io_service.run();

    return 0;
}

