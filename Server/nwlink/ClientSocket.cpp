#include "stdafx.h"
#include "ClientSocket.h"
#include "WorldLink.h"
#include "../Common/cmdcode.h"


void ClientSocket::ConnectHandler()
{
	_pworldLink->SendLoginInfo();
}

void ClientSocket::SendPacket(WorldPacket& packet)
{
	MessageBuffer buffer;
	uint16 uCmd = packet.GetOpcode();
	uint16 uLen = 0;
	buffer.Write(&uCmd, 2);
	uLen = packet.size();
	buffer.Write(&uLen, 2);
	buffer.Write(packet.contents(), packet.size());

	Write(buffer);
}

void ClientSocket::ReadHandler()
{
	if (!IsOpen())
		return;

	MessageBuffer& packet = GetReadBuffer();
	while (packet.GetActiveSize() > 0)
	{
		if (_headerBuffer.GetRemainingSpace() > 0)
		{
			// need to receive the header
			std::size_t readHeaderSize = std::min(packet.GetActiveSize(), _headerBuffer.GetRemainingSpace());
			_headerBuffer.Write(packet.GetReadPointer(), readHeaderSize);
			packet.ReadCompleted(readHeaderSize);

			if (_headerBuffer.GetRemainingSpace() > 0)
			{
				// Couldn't receive the whole header this time.
				//ASSERT(packet.GetActiveSize() == 0);
				break;
			}

			// We just received nice new header
			if (!ReadHeaderHandler())
			{
				CloseSocket();
				return;
			}
		}

		// We have full read header, now check the data payload
		if (_packetBuffer.GetRemainingSpace() > 0)
		{
			// need more data in the payload
			std::size_t readDataSize = std::min(packet.GetActiveSize(), _packetBuffer.GetRemainingSpace());
			_packetBuffer.Write(packet.GetReadPointer(), readDataSize);
			packet.ReadCompleted(readDataSize);

			if (_packetBuffer.GetRemainingSpace() > 0)
			{
				// Couldn't receive the whole data this time.
				//ASSERT(packet.GetActiveSize() == 0);
				break;
			}
		}

		// just received fresh new payload
		ReadDataHandlerResult result = ReadDataHandler();
		_headerBuffer.Reset();
		if (result != ReadDataHandlerResult::Ok)
		{
			if (result != ReadDataHandlerResult::WaitingForQuery)
				CloseSocket();

			return;
		}
	}

	AsyncRead();
}

ClientSocket::ReadDataHandlerResult ClientSocket::ReadDataHandler()
{
	ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(_headerBuffer.GetReadPointer());
	uint32 cmd;
	uint32 size;

	cmd = header->Nano.uCmd;
	size = header->Nano.uLen;

	WorldPacket packet(cmd, std::move(_packetBuffer), CONNECTION_TYPE_INSTANCE);

	switch (cmd)
	{
	case  SMSG_LOGINACK:
	{
		uint32 sID = 0;
		packet >> sID;
		_pworldLink->SessionID(sID);
		std::cout << "Sessoin " << sID << std::endl;
	}
		break;
	default:
		break;
	}


	return ReadDataHandlerResult::Ok;
}

bool ClientSocket::ReadHeaderHandler()
{
	//ASSERT(_headerBuffer.GetActiveSize() == SizeOfClientHeader[_initialized][_authCrypt.IsInitialized()], "Header size " SZFMTD " different than expected %u", _headerBuffer.GetActiveSize(), SizeOfClientHeader[_initialized][_authCrypt.IsInitialized()]);

	//_authCrypt.DecryptRecv(_headerBuffer.GetReadPointer(), _headerBuffer.GetActiveSize());

	ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(_headerBuffer.GetReadPointer());
	uint32 opcode;
	uint32 size;

	size = header->Nano.uLen;
	opcode = header->Nano.uCmd;

	//ExtractOpcodeAndSize(header, opcode, size);

	/*if (!ClientPktHeader::IsValidSize(size) || (_initialized && !ClientPktHeader::IsValidOpcode(opcode)))
	{
	TC_LOG_ERROR("network", "WorldSocket::ReadHeaderHandler(): client %s sent malformed packet (size: %u, cmd: %u)",
	GetRemoteIpAddress().to_string().c_str(), size, opcode);
	return false;
	}*/

	_packetBuffer.Resize(size);
	return true;
}