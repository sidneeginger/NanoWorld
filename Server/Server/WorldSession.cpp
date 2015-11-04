#include "stdafx.h"
#include "WorldSession.h"
#include "World.h"
#include "WorldSocket.h"
#include "WorldPacket.h"
#include "../Common/cmdcode.h"

WorldSession::WorldSession(uint32 sessionID, WorldSocket* pSocket)
	: _SessoinID(sessionID), _pSocket(pSocket), m_bPlayerKicked(false)
{
}


WorldSession::~WorldSession()
{
}

void WorldSession::SendPostoSet()
{
	sWorld->UpdateMove(this);
}

void WorldSession::SendOtherPlayerPos(uint32 sID, Player& player)
{
	WorldPacket packet;
	packet.SetOpcode(SMSG_MOVE_UPDATE);
	packet << sID;
	packet << player.Pos().x;
	packet << player.Pos().y;
	packet << player.Pos().z;
	packet << player.Pos().angle;

	_pSocket->SendPacket(packet);
}

void WorldSession::KickPlayer(uint32 uid)
{
	WorldPacket packet;
	packet.SetOpcode(SMSG_Player_Remove);
	packet << uid;

	_pSocket->SendPacket(packet);
}


void WorldSession::Update(uint32 udiff)
{
	
}
