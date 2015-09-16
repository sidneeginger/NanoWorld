#pragma once
#include "define.h"


enum  OptCode : uint16
{
	CMSG_BASE						= 0x0000,
	CMSG_LOGIN					= CMSG_BASE+1,
	CMSG_LOGOUT				= CMSG_BASE + 2,
	CMSG_MOVE_START		= CMSG_BASE + 3,
    CMSG_LINK           = CMSG_BASE + 4,

	SMSG_BASE						= 0xA000,
	SMSG_LOGINACK			= SMSG_BASE + 1,
	SMSG_MOVE_UPDATE	= SMSG_BASE + 2,
	SMSG_Player_Remove		= SMSG_BASE + 3,
};

