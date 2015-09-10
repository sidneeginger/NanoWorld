#include "stdafx.h"
#include "WorldLink.h"
#include "../Common/cmdcode.h"

CWorldLink::~CWorldLink()
{
}


void CWorldLink::SendPlayerPos(float fx, float fy, float fz, float fa)
{
	WorldPacket packet;
	packet.SetOpcode(CMSG_MOVE_START);
	packet << fx;
	packet << fy;
	packet << fz;
	packet << fa;

	SendPacket(packet);
}


void CWorldLink::SendLoginInfo()
{
	WorldPacket packet;
	packet.SetOpcode(CMSG_LOGIN);
	packet << uint32(12345);

	SendPacket(packet);
}

void CWorldLink::SendLogout()
{
	WorldPacket packet;
	packet.SetOpcode(CMSG_LOGOUT);
	packet << uint32(12345);

	SendPacket(packet);
}