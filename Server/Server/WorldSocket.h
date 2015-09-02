#pragma once
#include "../Common/Socket.h"

class WorldSocket : public Socket<WorldSocket>
{
public:
	explicit WorldSocket(tcp::socket&& socket);
	~WorldSocket();

public:
	void Start() override;

protected:
	void ReadHandler() override;
public:
	int WriteLoginInfo();
};

