#include "stdafx.h"
#include "WorldSocket.h"
#include <iostream>
#include "bitstream.h"
#include "WorldPacket.h"
#include "../Common/cmdcode.h"
#include "WorldSession.h"
#include "World.h"
#include "../database/DatabaseEnv.h"
#include "log.h"

WorldSocket::WorldSocket(tcp::socket&& socket) 
	: Socket(std::move(socket))
{
	_headerBuffer.Resize(4);
	_session = nullptr;
}


WorldSocket::~WorldSocket()
{
}

void WorldSocket::Start()
{
	std::string ip_address = GetRemoteIpAddress().to_string();
	auto nPort = GetRemotePort();
	std::cout << ip_address << ":" << nPort << std::endl;

	AsyncRead();
}

void WorldSocket::ReadHandler()
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

void WorldSocket::Write(MessageBuffer& buffer)
{
	std::unique_lock<std::mutex> guard(_writeLock);
	QueuePacket(std::move(buffer), guard);
}


void WorldSocket::SendPacket(WorldPacket& packet)
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

int WorldSocket::WriteLoginInfo(uint32 uSession)
{
	WorldPacket packet;
	packet.SetOpcode(SMSG_LOGINACK);
	packet << uSession;

	SendPacket(packet);
	return 0;
}


bool WorldSocket::ReadHeaderHandler()
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


WorldSocket::ReadDataHandlerResult WorldSocket::ReadDataHandler()
{
	ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(_headerBuffer.GetReadPointer());
	uint32 cmd;
	uint32 size;

	cmd = header->Nano.uCmd;
	size = header->Nano.uLen;

	WorldPacket packet(cmd, std::move(_packetBuffer), CONNECTION_TYPE_INSTANCE);

	switch (cmd)
	{
	case CMSG_LOGOUT:
	{
		// Logout
		uint32 uID;
		packet >> uID;
		//_session->KickPlayer(_session->SessoinID());
		sWorld->KickedPlayer(_session);
		std::cout << "Player LogOut " << uID << std::endl;
	}
		break;
	case CMSG_LOGIN:
	{
		HandleLogin(packet);
	}
		break;
	case CMSG_MOVE_START:
		{
			Position pos;
		
			packet >> pos.x;
			packet >> pos.y;
			packet >> pos.z;
			packet >> pos.angle;

			_session->SetPlayerPos(pos);
			//std::cout << fx << " " << fy << " " << fz << " " << fa <<std::endl;
		}
		break;
	default:
		break;
	}

	//if (_initialized)
	//{
	//	ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(_headerBuffer.GetReadPointer());
	//	uint32 cmd;
	//	uint32 size;

	//	ExtractOpcodeAndSize(header, cmd, size);

	//	OpcodeClient opcode = static_cast<OpcodeClient>(cmd);

	//	WorldPacket packet(opcode, std::move(_packetBuffer), GetConnectionType());

	//	if (sPacketLog->CanLogPacket())
	//		sPacketLog->LogPacket(packet, CLIENT_TO_SERVER, GetRemoteIpAddress(), GetRemotePort(), GetConnectionType());

	//	std::unique_lock<std::mutex> sessionGuard(_worldSessionLock, std::defer_lock);

	//	switch (opcode)
	//	{
	//	case CMSG_PING:
	//		LogOpcodeText(opcode, sessionGuard);
	//		return HandlePing(packet) ? ReadDataHandlerResult::Ok : ReadDataHandlerResult::Error;
	//	case CMSG_AUTH_SESSION:
	//	{
	//		LogOpcodeText(opcode, sessionGuard);
	//		if (_authed)
	//		{
	//			// locking just to safely log offending user is probably overkill but we are disconnecting him anyway
	//			if (sessionGuard.try_lock())
	//				TC_LOG_ERROR("network", "WorldSocket::ProcessIncoming: received duplicate CMSG_AUTH_SESSION from %s", _worldSession->GetPlayerInfo().c_str());
	//			return ReadDataHandlerResult::Error;
	//		}

	//		std::shared_ptr<WorldPackets::Auth::AuthSession> authSession = std::make_shared<WorldPackets::Auth::AuthSession>(std::move(packet));
	//		authSession->Read();
	//		HandleAuthSession(authSession);
	//		return ReadDataHandlerResult::WaitingForQuery;
	//	}
	//	case CMSG_AUTH_CONTINUED_SESSION:
	//	{
	//		LogOpcodeText(opcode, sessionGuard);
	//		if (_authed)
	//		{
	//			// locking just to safely log offending user is probably overkill but we are disconnecting him anyway
	//			if (sessionGuard.try_lock())
	//				TC_LOG_ERROR("network", "WorldSocket::ProcessIncoming: received duplicate CMSG_AUTH_CONTINUED_SESSION from %s", _worldSession->GetPlayerInfo().c_str());
	//			return ReadDataHandlerResult::Error;
	//		}

	//		std::shared_ptr<WorldPackets::Auth::AuthContinuedSession> authSession = std::make_shared<WorldPackets::Auth::AuthContinuedSession>(std::move(packet));
	//		authSession->Read();
	//		HandleAuthContinuedSession(authSession);
	//		return ReadDataHandlerResult::WaitingForQuery;
	//	}
	//	case CMSG_KEEP_ALIVE:
	//		LogOpcodeText(opcode, sessionGuard);
	//		break;
	//	case CMSG_LOG_DISCONNECT:
	//		LogOpcodeText(opcode, sessionGuard);
	//		packet.rfinish();   // contains uint32 disconnectReason;
	//		break;
	//	case CMSG_ENABLE_NAGLE:
	//		LogOpcodeText(opcode, sessionGuard);
	//		SetNoDelay(false);
	//		break;
	//	case CMSG_CONNECT_TO_FAILED:
	//	{
	//		sessionGuard.lock();

	//		LogOpcodeText(opcode, sessionGuard);
	//		WorldPackets::Auth::ConnectToFailed connectToFailed(std::move(packet));
	//		connectToFailed.Read();
	//		HandleConnectToFailed(connectToFailed);
	//		break;
	//	}
	//	default:
	//	{
	//		sessionGuard.lock();

	//		LogOpcodeText(opcode, sessionGuard);

	//		if (!_worldSession)
	//		{
	//			TC_LOG_ERROR("network.opcode", "ProcessIncoming: Client not authed opcode = %u", uint32(opcode));
	//			return ReadDataHandlerResult::Error;
	//		}

	//		OpcodeHandler const* handler = opcodeTable[opcode];
	//		if (!handler)
	//		{
	//			TC_LOG_ERROR("network.opcode", "No defined handler for opcode %s sent by %s", GetOpcodeNameForLogging(static_cast<OpcodeClient>(packet.GetOpcode())).c_str(), _worldSession->GetPlayerInfo().c_str());
	//			break;
	//		}

	//		// Our Idle timer will reset on any non PING opcodes.
	//		// Catches people idling on the login screen and any lingering ingame connections.
	//		_worldSession->ResetTimeOutTime();

	//		// Copy the packet to the heap before enqueuing
	//		_worldSession->QueuePacket(new WorldPacket(std::move(packet)));
	//		break;
	//	}
	//	}
	//}
	//else
	//{
	//	std::string initializer(reinterpret_cast<char const*>(_packetBuffer.GetReadPointer()), std::min(_packetBuffer.GetActiveSize(), ClientConnectionInitialize.length()));
	//	if (initializer != ClientConnectionInitialize)
	//		return ReadDataHandlerResult::Error;

	//	_compressionStream = new z_stream();
	//	_compressionStream->zalloc = (alloc_func)NULL;
	//	_compressionStream->zfree = (free_func)NULL;
	//	_compressionStream->opaque = (voidpf)NULL;
	//	_compressionStream->avail_in = 0;
	//	_compressionStream->next_in = NULL;
	//	int32 z_res = deflateInit2(_compressionStream, sWorld->getIntConfig(CONFIG_COMPRESSION), Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
	//	if (z_res != Z_OK)
	//	{
	//		TC_LOG_ERROR("network", "Can't initialize packet compression (zlib: deflateInit) Error code: %i (%s)", z_res, zError(z_res));
	//		return ReadDataHandlerResult::Error;
	//	}

	//	_initialized = true;
	//	_headerBuffer.Resize(SizeOfClientHeader[1][0]);
	//	_packetBuffer.Reset();
	//	HandleSendAuthSession();
	//}

	return ReadDataHandlerResult::Ok;
}

void WorldSocket::HandleLogin(WorldPacket& packet)
{
	if (_queryCallback)
	{
		TC_LOG_DEBUG("session", "[Battlenet::ResumeRequest] %s attempted to log too quick after previous attempt!", GetRemoteIpAddress().to_string().c_str());
		return;
	}

	// Login
	uint32 uID;
	std::string sName;
	std::string sPW;
	packet >> sName;
	packet >> sPW;
	std::cout << "Player Login " << sName.c_str() << " PW" << sPW.c_str() << std::endl;


	PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_NW);
	stmt->setString(0, sName);
	stmt->setString(1, sPW);

	_queryCallback = std::bind(&WorldSocket::HandleResumeRequestCallback, this, std::placeholders::_1);
	_queryFuture = LoginDatabase.AsyncQuery(stmt);
}

void WorldSocket::HandleResumeRequestCallback(PreparedQueryResult result)
{
	if (!result)
	{
		return;
	}

	Field* fields = result->Fetch();

	auto id = fields[0].GetInt32();

	NewSession(id);
}


void WorldSocket::NewSession(uint32 AccountID)
{
	_session = new WorldSession(AccountID, this);
	if (_session != nullptr)
	{
		sWorld->AddSession(_session);
		WriteLoginInfo(AccountID);

		// Get player pos;
		Position pos;
		_session->SetPlayerPos(pos);
	}
}

bool WorldSocket::Update()
{
	if (!NanoNetSocket::Update())
		return false;

	if (_queryFuture.valid() && _queryFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		auto callback = std::move(_queryCallback);
		_queryCallback = nullptr;
		callback(_queryFuture.get());
	}

	return true;
}