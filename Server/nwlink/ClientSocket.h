#pragma once

#include "../Common/Socket.h"

class CWorldLink;
class ClientSocket : public Socket<ClientSocket>
{
public:
	explicit ClientSocket(tcp::socket&& socket)
		: Socket(std::move(socket), true)
	{

	}
	~ClientSocket()
	{

	}

public:
	void Write(MessageBuffer& buffer)
	{
		std::unique_lock<std::mutex> guard(_writeLock);
		QueuePacket(std::move(buffer), guard);
	}

	void Start() override
	{

	}

	void SetWorldLink(std::shared_ptr<CWorldLink> p) { _pworldLink = p; };

	virtual void ConnectHandler();

protected:
	void ReadHandler() override
	{
		// read server info
	}
	bool ReadHeaderHandler()
	{

	}

private:
	MessageBuffer _headerBuffer;
	MessageBuffer _packetBuffer;
	std::shared_ptr<CWorldLink> _pworldLink;
};
