#pragma once
#include "common.h"
#include <atomic>
#include <map>
#include <set>
#include <list>

enum ShutdownExitCode
{
	SHUTDOWN_EXIT_CODE = 0,
	ERROR_EXIT_CODE = 1,
	RESTART_EXIT_CODE = 2
};

class World
{
public:
	static World* instance()
	{
		static World instance;
		return &instance;
	}
	static std::atomic<uint32> m_worldLoopCounter;

	static bool IsStopped() { return m_stopEvent; }
	void Update(uint32 diff);
	static void StopNow(uint8 exitcode) { m_stopEvent = true; m_ExitCode = exitcode; }

private:
	World();
	~World();

	static std::atomic<bool> m_stopEvent;
	static uint8 m_ExitCode;
	uint32 m_updateTime, m_updateTimeSum;
};

#define sWorld World::instance()