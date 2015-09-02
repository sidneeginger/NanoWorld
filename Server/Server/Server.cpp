// Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include <boost/asio.hpp>
//
//static boost::asio::io_service* _ioService;
//
//void SignalHandler(const boost::system::error_code& error, int signalNumber);
//
//int main()
//{
//	_ioService = new boost::asio::io_service();
//
//	// Set signal handlers
//	boost::asio::signal_set signals(*_ioService, SIGINT, SIGTERM);
//#if PLATFORM == PLATFORM_WINDOWS
//	signals.add(SIGBREAK);
//#endif
//	signals.async_wait(SignalHandler);
//
//	//_ioService->run();
//
//    return 0;
//}
//
//void SignalHandler(const boost::system::error_code& error, int /*signalNumber*/)
//{
//	if (!error)
//		_ioService->stop();
//}

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>


#include "WorldSocketMgr.h"


int main()
{
	boost::asio::io_service io;

	sWorldMgr.StartNetwork(io, "127.0.0.1", 5758);
	std::cout << "World Manager Start." << std::endl;
	io.run();

	return 0;
}