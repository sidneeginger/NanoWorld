#pragma once
#include "../Common/Socket.h"
#include "../Common/Packet.h"
#include "../Common/Threading/Callback.h"

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
	typedef Socket<WorldSocket> NanoNetSocket;
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
	std::string GetClientInfo() const;
	int WriteLoginInfo(uint32);
	void Write(MessageBuffer& buffer);

	void SendPacket(WorldPacket& packet);
	bool Update() override;

private:
	MessageBuffer _headerBuffer;
	MessageBuffer _packetBuffer;
	WorldSession* _session;

	PreparedQueryResultFuture _queryFuture;
	std::function<void(PreparedQueryResult)> _queryCallback;

	void HandleLogin(WorldPacket& packet);
	void HandleResumeRequestCallback(PreparedQueryResult result);
	void NewSession(uint32 AccountID);
};

