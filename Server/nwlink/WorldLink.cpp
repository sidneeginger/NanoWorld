#include "stdafx.h"
#include "WorldLink.h"


CWorldLink::~CWorldLink()
{
}


void CWorldLink::SendPlayerPos(float fx, float fy, float fz, float fa)
{
	MessageBuffer buffer;
	uint16 uCmd = 0x1A;
	uint16 uLen = 0;
	buffer.Write(&uCmd, 2);

	WorldPacket packet;
	packet << fx;
	packet << fy;
	packet << fz;
	packet << fa;

	uLen = packet.size();
	buffer.Write(&uLen, 2);
	buffer.Write(packet.contents(), packet.size());

	auto len = buffer.GetActiveSize();

	pClient->Write(buffer);
}


void CWorldLink::SendLoginInfo()
{
	MessageBuffer buffer;
	uint16 uCmd = 0x1B;
	uint16 uLen = 0;
	buffer.Write(&uCmd, 2);

	WorldPacket packet;
	packet << uint32(12345);

	uLen = packet.size();
	buffer.Write(&uLen, 2);
	buffer.Write(packet.contents(), packet.size());

	pClient->Write(buffer);
}

void CWorldLink::SendLogout()
{
	MessageBuffer buffer;
	uint16 uCmd = 0x1C;
	uint16 uLen = 0;
	buffer.Write(&uCmd, 2);

	WorldPacket packet;
	packet << uint32(12345);

	uLen = packet.size();
	buffer.Write(&uLen, 2);
	buffer.Write(packet.contents(), packet.size());

	pClient->Write(buffer);
}