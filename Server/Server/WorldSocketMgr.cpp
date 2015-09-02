#include "stdafx.h"
#include "WorldSocketMgr.h"


WorldSocketMgr::WorldSocketMgr()
{
}


WorldSocketMgr::~WorldSocketMgr()
{
}



NetworkThread<WorldSocket>* WorldSocketMgr::CreateThreads() const
{
	return new NetworkThread<WorldSocket>[GetNetworkThreadCount()];
}


bool WorldSocketMgr::StartNetwork(boost::asio::io_service& service, std::string const& bindIp, uint16 port)
{
	if (!BaseSocketMgr::StartNetwork(service, bindIp, port))
		return false;

	_acceptor->AsyncAcceptManaged(&OnSocketAccept);
	return true;
}

void WorldSocketMgr::OnSocketAccept(tcp::socket&& sock)
{
	sWorldMgr.OnSocketOpen(std::forward<tcp::socket>(sock));
}