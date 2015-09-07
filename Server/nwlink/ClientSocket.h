#pragma once

#include "../Common/Socket.h"


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
	void Write()
	{
		
	}

	void Start() override
	{

	}

protected:
	void ReadHandler() override
	{

	}
	bool ReadHeaderHandler()
	{

	}

private:
	MessageBuffer _headerBuffer;
	MessageBuffer _packetBuffer;
};
