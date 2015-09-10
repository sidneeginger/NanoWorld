#pragma once
#include "define.h"

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