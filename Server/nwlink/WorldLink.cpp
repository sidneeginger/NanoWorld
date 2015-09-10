#include "stdafx.h"
#include "WorldLink.h"
#include "../Common/cmdcode.h"

CWorldLink::~CWorldLink()
{
}


void CWorldLink::SendPlayerPos(float fx, float fy, float fz, float fa)
{
	WorldPacket packet;
	packet.SetOpcode(CMSG_MOVE_START);
	packet << fx;
	packet << fy;
	packet << fz;
	packet << fa;

	SendPacket(packet);
}


void CWorldLink::SendLoginInfo()
{
	WorldPacket packet;
	packet.SetOpcode(CMSG_LOGIN);
	packet << uint32(12345);

	SendPacket(packet);
}

void CWorldLink::SendLogout()
{
	WorldPacket packet;
	packet.SetOpcode(CMSG_LOGOUT);
	packet << uint32(12345);

	SendPacket(packet);
}

void CWorldLink::NofifyLogin()
{
	SendPlayerPos(0, 0, 0, 0);
}

void CWorldLink::RemovePlayer(uint32 uid)
{
	std::unique_lock<std::mutex> guard(_ObjectLock);
	auto find = m_mapObject.find(uid);
	if (find != m_mapObject.end())
	{
		delete find->second;
		m_listRemove.push_back(find->first);
		m_mapObject.erase(find->first);

	}
}

void CWorldLink::UpdateObjectMove(uint32 uid, float x, float y, float z, float a)
{
	std::unique_lock<std::mutex> guard(_ObjectLock);
	auto find = m_mapObject.find(uid);
	if (find == m_mapObject.end())
	{
		auto ob = new Object();
		ob->uid = uid;
		ob->x = x;
		ob->y = y;
		ob->z = z;
		ob->a = a;
		m_mapObject[uid] = ob;
	}
	else
	{
		auto ob = find->second;
		ob->uid = uid;
		ob->x = x;
		ob->y = y;
		ob->z = z;
		ob->a = a;
	}
}

ObjectList CWorldLink::GetObjectList()
{
	ObjectList list;
	std::unique_lock<std::mutex> guard(_ObjectLock);
	for (auto& ob : m_mapObject)
	{
		Object lOB = *ob.second;
		list.push_back(lOB);
	}

	return list;
}

IDList CWorldLink::GetRemoveList()
{
	std::unique_lock<std::mutex> guard(_ObjectLock);
	return m_listRemove;
}

void CWorldLink::CleanRemoveList()
{
	std::unique_lock<std::mutex> guard(_ObjectLock);
	m_listRemove.clear();
}