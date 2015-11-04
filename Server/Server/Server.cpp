// Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>

#include "GitRevision.h"
#include "DatabaseEnv.h"
#include "WorldSocketMgr.h"
#include "World.h"
#include "DatabaseLoader.h"
#include "Config/Config.h"
#include <boost/program_options.hpp>
#include "AppenderDB.h"
#include <openssl/opensslv.h>
#include <openssl/crypto.h>


using namespace boost::program_options;

#ifndef _TRINITY_BNET_CONFIG
# define _TRINITY_BNET_CONFIG  "bnetserver.conf"
#endif


#define WORLD_SLEEP_CONST 50

LoginDatabaseWorkerPool LoginDatabase;

void SignalHandler(const boost::system::error_code& error, int /*signalNumber*/);
void WorldUpdateLoop();
bool StartDB();
void StopDB();
variables_map GetConsoleArguments(int argc, char** argv, std::string& configFile, std::string& configService);

int main(int argc, char** argv)
{
	std::string configFile = _TRINITY_BNET_CONFIG;
	std::string configService;
	auto vm = GetConsoleArguments(argc, argv, configFile, configService);
	// exit if help or version is enabled
	if (vm.count("help") || vm.count("version"))
		return 0;

	std::string configError;
	if (!sConfigMgr->LoadInitial(configFile, configError))
	{
		printf("Error in config file: %s\n", configError.c_str());
		return 1;
	}

	sLog->RegisterAppender<AppenderDB>();
	sLog->Initialize(nullptr);

	TC_LOG_INFO("server.bnetserver", "%s (bnetserver)", GitRevision::GetFullVersion());
	TC_LOG_INFO("server.bnetserver", "<Ctrl-C> to stop.\n");
	TC_LOG_INFO("server.bnetserver", "Using configuration file %s.", configFile.c_str());
	TC_LOG_INFO("server.bnetserver", "Using SSL version: %s (library: %s)", OPENSSL_VERSION_TEXT, SSLeay_version(SSLEAY_VERSION));
	TC_LOG_INFO("server.bnetserver", "Using Boost version: %i.%i.%i", BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100);

	std::string pidFile = sConfigMgr->GetStringDefault("PidFile", "");
	if (!pidFile.empty())
	{
		if (uint32 pid = CreatePIDFile(pidFile))
			TC_LOG_INFO("server.bnetserver", "Daemon PID: %u\n", pid);
		else
		{
			TC_LOG_ERROR("server.bnetserver", "Cannot create PID file %s.\n", pidFile.c_str());
			return 1;
		}
	}

	int32 worldListenPort = sConfigMgr->GetIntDefault("WorldserverListenPort", 1118);
	if (worldListenPort < 0 || worldListenPort > 0xFFFF)
	{
		TC_LOG_ERROR("server.bnetserver", "Specified worldserver listen port (%d) out of allowed range (1-65535)", worldListenPort);
		return 1;
	}

	// Start the listening port (acceptor) for auth connections
	int32 bnport = sConfigMgr->GetIntDefault("BattlenetPort", 5758);
	if (bnport < 0 || bnport > 0xFFFF)
	{
		TC_LOG_ERROR("server.bnetserver", "Specified battle.net port (%d) out of allowed range (1-65535)", bnport);
		return 1;
	}

	std::string bindIp = sConfigMgr->GetStringDefault("BindIP", "0.0.0.0");
	if (StartDB() == false)
	{
		return 1;
	}


	boost::asio::io_service io;
	sWorldMgr.StartNetwork(io, bindIp, bnport);
	std::cout << "World Manager Start." << std::endl;

	boost::asio::signal_set signals(io, SIGINT, SIGTERM);
#ifdef _WIN32
#if PLATFORM == PLATFORM_WINDOWS
	signals.add(SIGBREAK);
#endif
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

	StopDB();

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

bool StartDB()
{
	MySQL::Library_Init();
	// Load databases
	DatabaseLoader loader("server.bnetserver", DatabaseLoader::DATABASE_NONE);
	loader
		.AddDatabase(LoginDatabase, "Login");

	if (!loader.Load())
		return false;

	return true;
}

void StopDB()
{
	LoginDatabase.Close();
	MySQL::Library_End();
}

variables_map GetConsoleArguments(int argc, char** argv, std::string& configFile, std::string& configService)
{
	(void)configService;

	options_description all("Allowed options");
	all.add_options()
		("help,h", "print usage message")
		("version,v", "print version build info")
		("config,c", value<std::string>(&configFile)->default_value(_TRINITY_BNET_CONFIG), "use <arg> as configuration file")
		;
#if PLATFORM == PLATFORM_WINDOWS
	options_description win("Windows platform specific options");
	win.add_options()
		("service,s", value<std::string>(&configService)->default_value(""), "Windows service options: [install | uninstall]")
		;

	all.add(win);
#endif
	variables_map variablesMap;
	try
	{
		store(command_line_parser(argc, argv).options(all).allow_unregistered().run(), variablesMap);
		notify(variablesMap);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}

	if (variablesMap.count("help"))
	{
		std::cout << all << "\n";
	}
	else if (variablesMap.count("version"))
	{
		std::cout << GitRevision::GetFullVersion() << "\n";
	}

	return variablesMap;
}