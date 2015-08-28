#pragma once
#include "../Common/SocketMgr.h"
#include "WorldSocket.h"

class WorldSocketMgr  : public SocketMgr<WorldSocket>
{
public:
	WorldSocketMgr();
	~WorldSocketMgr();
};

