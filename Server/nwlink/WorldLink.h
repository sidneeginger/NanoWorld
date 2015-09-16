#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <thread>
#include "ClientSocket.h"
#include "../Common/WorldPacket.h"
#include "../Common/MessageBuffer.h"
#include "../Common/Timer.h"
#include <unordered_map>
#include <vector>
#include <iostream>

struct Object
{
	uint32 uid;
	float x;
	float y;
	float z;
	float a;
};

typedef std::unordered_map<uint32, Object*> ObjectMap;
typedef std::vector<Object> ObjectList;
typedef std::vector<uint32> IDList;

class CWorldLink
{
public:
	CWorldLink(boost::asio::io_service& io) 
		: socket_(io), io_service_(io)
	{
		pClient = std::make_shared<ClientSocket>(std::move(socket_));
	}
	~CWorldLink();

	uint32 SessionID() const { return _sessionID; }
	void SessionID(uint32 val) { _sessionID = val; }

public:

private:
	boost::asio::io_service& io_service_;
	boost::asio::ip::tcp::socket socket_;
	std::shared_ptr<ClientSocket> pClient;
	uint32 _sessionID;
	std::mutex _ObjectLock;
	ObjectMap m_mapObject;
	IDList m_listRemove;

	void SendPacket(WorldPacket& packet)
	{
		pClient->SendPacket(packet);
	}

public:

	void Init()
	{
		pClient->SetWorldLink(std::shared_ptr<CWorldLink>(this));
		pClient->Connect("127.0.0.1", "5758");
	}

	void SendPlayerPos(float fx, float fy, float fz, float fa);
    void SendLoginInfo(std::string& name, std::string& pw);
    void SendLink();
	void SendLogout();
	void UpdateObjectMove(uint32 uid, float x, float y, float z, float a);
	ObjectList GetObjectList();
	void NofifyLogin();
	void RemovePlayer(uint32 uid);
	IDList GetRemoveList();
	void CleanRemoveList();

};

