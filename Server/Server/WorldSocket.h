#pragma once
#include "../Common/Socket.h"
#include "../Common/Packet.h"

class WorldSession;
class WorldPacket;
enum class BufferSizes : uint32
{
	SRP_6_V = 0x80,
	SRP_6_S = 0x20,
	Read = 0x4000
};

class WorldSocket : public Socket<WorldSocket>
{
public:
	explicit WorldSocket(tcp::socket&& socket);
	~WorldSocket();

public:
	void Start() override;

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

public:
	int WriteLoginInfo(uint32);
	void Write(MessageBuffer& buffer);

	void SendPacket(WorldPacket& packet);

private:
	MessageBuffer _headerBuffer;
	MessageBuffer _packetBuffer;
	WorldSession* _session;
};

