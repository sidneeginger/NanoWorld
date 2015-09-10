#pragma once
#include "define.h"


enum  ClientOptCode : uint16
{
	CMSG_LOGIN = 0x001A,
	CMSG_LOGOUT = 0x001B,
	CMSG_MOVE_START = 0x001C,

	SMSG_LOGINACK = 0xA01A
};

