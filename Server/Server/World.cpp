#include "stdafx.h"
#include "World.h"

std::atomic<bool> World::m_stopEvent(false);
uint8 World::m_ExitCode = SHUTDOWN_EXIT_CODE;
std::atomic<uint32> World::m_worldLoopCounter(0);

World::World()
{
}


World::~World()
{
}


void World::Update(uint32 diff)
{
	m_updateTime = diff;
}