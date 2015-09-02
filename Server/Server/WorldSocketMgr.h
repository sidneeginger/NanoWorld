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
	bool StartNetwork(boost::asio::io_service& service, std::string const& bindIp, uint16 port) override;

protected:
	NetworkThread<WorldSocket>* CreateThreads() const override;

private:
	static void OnSocketAccept(tcp::socket&& sock);

};

