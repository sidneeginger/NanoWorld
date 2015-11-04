#include "stdafx.h"
#include "World.h"
#include "Player.h"

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
	UpdateSession(diff);
}

void World::UpdateSession(uint32 diff)
{
	WorldSession* sess = NULL;
	while (addSessQueue.next(sess))
		AddSession_(sess);

	for (auto& it : m_session)
	{
		it.second->Update(diff);
		if (it.second->PlayerKicked())
		{
			m_session.erase(it.first);
			break;
		}
	}
}

void World::AddSession(WorldSession* s)
{
	addSessQueue.add(s);
}

void World::AddSession_(WorldSession* s)
{
	auto find = m_session.find(s->SessoinID());
	if (find == m_session.end())
	{
		m_session[s->SessoinID()] = s;
		UpdateAllMove();
	}
	else
	{
		KickedPlayer(s);
	}
}

void World::UpdateAllMove()
{
	for (auto& it : m_session)
	{
		UpdateMove(it.second);
	}
}


void World::KickedPlayer(WorldSession* s)
{
	if (nullptr == s)
	{
		return;
	}

	Player player = s->GetPlayer();
	for (auto& it : m_session)
	{
		if (it.first != s->SessoinID())
		{
			it.second->KickPlayer(s->SessoinID());
		}
	}
	s->PlayerKicked(true);
}

void World::UpdateMove(WorldSession* s)
{
	Player player = s->GetPlayer();
	for (auto& it : m_session)
	{
		if (it.first != s->SessoinID())
		{
			it.second->SendOtherPlayerPos(s->SessoinID(), player);
		}
	}
}