#pragma once
#include "../Common/Socket.h"

enum class BufferSizes : uint32
{
	SRP_6_V = 0x80,
	SRP_6_S = 0x20,
	Read = 0x4000
};

#pragma pack(push, 1)

union ClientPktHeader
{
	struct
	{
		uint16 Size;
		uint32 Command;
	} Setup;

	struct
	{
		uint32 Command : 13;
		uint32 Size : 19;
	} Normal;

	struct  
	{
		uint16 uCmd;
		uint16 uLen;
	} Nano;

	static bool IsValidSize(uint32 size) { return size < 10240; }
	static bool IsValidOpcode(uint32 opcode) { return opcode < 1024; }
};

#pragma pack(pop)

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
	int WriteLoginInfo();

private:
	MessageBuffer _headerBuffer;
	MessageBuffer _packetBuffer;
};

