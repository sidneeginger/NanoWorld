// Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>

#include "WorldSocketMgr.h"
#include "World.h"

#define WORLD_SLEEP_CONST 50

void SignalHandler(const boost::system::error_code& error, int /*signalNumber*/);
void WorldUpdateLoop();

int main()
{
	boost::asio::io_service io;

	sWorldMgr.StartNetwork(io, "127.0.0.1", 5758);
	std::cout << "World Manager Start." << std::endl;

	boost::asio::signal_set signals(io, SIGINT, SIGTERM);
#ifdef _WIN32
//#if PLATFORM == PLATFORM_WINDOWS
	signals.add(SIGBREAK);
#endif
    
	signals.async_wait(SignalHandler);

	int numThreads = 1;
	std::vector<std::thread> threadPool;

	if (numThreads < 1)
		numThreads = 1;

	for (int i = 0; i < numThreads; ++i)
		threadPool.push_back(std::thread(boost::bind(&boost::asio::io_service::run, &io)));

	WorldUpdateLoop();

	sWorldMgr.StopNetwork();

	return 0;
}

void SignalHandler(const boost::system::error_code& error, int /*signalNumber*/)
{
	if (!error)
		World::StopNow(SHUTDOWN_EXIT_CODE);
}

void WorldUpdateLoop()
{
	uint32 realCurrTime = 0;
	uint32 realPrevTime = getMSTime();

	uint32 prevSleepTime = 0;                               // used for balanced full tick time length near WORLD_SLEEP_CONST

															///- While we have not World::m_stopEvent, update the world
	while (!World::IsStopped())
	{
		++World::m_worldLoopCounter;
		realCurrTime = getMSTime();

		uint32 diff = getMSTimeDiff(realPrevTime, realCurrTime);

		sWorld->Update(diff);
		realPrevTime = realCurrTime;

		// diff (D0) include time of previous sleep (d0) + tick time (t0)
		// we want that next d1 + t1 == WORLD_SLEEP_CONST
		// we can't know next t1 and then can use (t0 + d1) == WORLD_SLEEP_CONST requirement
		// d1 = WORLD_SLEEP_CONST - t0 = WORLD_SLEEP_CONST - (D0 - d0) = WORLD_SLEEP_CONST + d0 - D0
		if (diff <= WORLD_SLEEP_CONST + prevSleepTime)
		{
			prevSleepTime = WORLD_SLEEP_CONST + prevSleepTime - diff;

			std::this_thread::sleep_for(std::chrono::milliseconds(prevSleepTime));
		}
		else
			prevSleepTime = 0;

//#ifdef _WIN32
//		if (m_ServiceStatus == 0)
//			World::StopNow(SHUTDOWN_EXIT_CODE);
//
//		while (m_ServiceStatus == 2)
//			Sleep(1000);
//#endif
	}
}