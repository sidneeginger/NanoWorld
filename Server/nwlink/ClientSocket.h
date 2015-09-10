#pragma once

#include "../Common/Socket.h"
#include "../Common/Packet.h"

class CWorldLink;
class WorldPacket;
class ClientSocket : public Socket<ClientSocket>
{
public:
	explicit ClientSocket(tcp::socket&& socket)
		: Socket(std::move(socket), true)
	{
		_headerBuffer.Resize(4);
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

	void SendPacket(WorldPacket& packet);

	void Start() override
	{

	}

	void SetWorldLink(std::shared_ptr<CWorldLink> p) { _pworldLink = p; };

	virtual void ConnectHandler();

protected:
	void ReadHandler() override;
	bool ReadHeaderHandler();
	enum class ReadDataHandlerResult
	{
		Ok = 0,
		Error = 1,
		WaitingForQuery = 2
	};

	ReadDataHandlerResult ReadDataHandler();

private:
	MessageBuffer _headerBuffer;
	MessageBuffer _packetBuffer;
	std::shared_ptr<CWorldLink> _pworldLink;
};
