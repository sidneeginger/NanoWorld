#pragma once
#include "../Common/SocketMgr.h"
#include "WorldSocket.h"

class WorldSocketMgr  : public SocketMgr<WorldSocket>
{
	typedef SocketMgr<WorldSocket> BaseSocketMgr;

public:
	WorldSocketMgr();
	~WorldSocketMgr();

public:
	static WorldSocketMgr& Instance()
	{
		static WorldSocketMgr instance;
		return instance;
	}
	bool StartNetwork(boost::asio::io_service& service, std::string const& bindIp, uint16 port) override;

protected:
	NetworkThread<WorldSocket>* CreateThreads() const override;

private:
	static void OnSocketAccept(tcp::socket&& sock);

};

#define sWorldMgr WorldSocketMgr::Instance()