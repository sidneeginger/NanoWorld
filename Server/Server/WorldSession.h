#pragma once
#include "../Common/define.h"
#include "Player.h"
class WorldSocket;
class WorldSession
{
public:
	WorldSession(uint32 sessionID, WorldSocket* pSocket);
	~WorldSession();
	uint32 SessoinID() const { return _SessoinID; }
	
	Player& GetPlayer() { return m_player; }

private:
	uint32 _SessoinID;
	Player m_player;
	WorldSocket* _pSocket;

public:
	void SetPlayerPos(Position pos)
	{
		m_player.Pos(pos);
		SendPostoSet();
	}

	void SendOtherPlayerPos(uint32 sID, Player& player);

protected:
	void SendPostoSet();

};

