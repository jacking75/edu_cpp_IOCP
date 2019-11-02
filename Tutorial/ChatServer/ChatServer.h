#pragma once

#include "./ServerNetwork/IOCPServer.h"
#include "UserManager.h"
#include "RoomManager.h"
#include "PacketManager.h"
#include "Packet.h"

#include <vector>
#include <deque>
#include <memory>
#include <thread>
#include <mutex>

//TODO redis 연동. hiredis 포함하기

class ChatServer : public IOCPServer
{
public:
	ChatServer() = default;
	virtual ~ChatServer() = default;
	

	virtual void OnConnect(const UINT32 clientIndex_) override 
	{
		printf("[OnConnect] 클라이언트: Index(%d)\n", clientIndex_);

		PacketInfo packet{ clientIndex_, (UINT16)PACKET_ID::SYS_USER_CONNECT, 0 };
		m_pPacketManager->PushSystemPacket(packet);
	}

	virtual void OnClose(const UINT32 clientIndex_) override 
	{
		printf("[OnClose] 클라이언트: Index(%d)\n", clientIndex_);

		PacketInfo packet{ clientIndex_, (UINT16)PACKET_ID::SYS_USER_DISCONNECT, 0 };
		m_pPacketManager->PushSystemPacket(packet);
	}

	virtual void OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) override  
	{
		printf("[OnReceive] 클라이언트: Index(%d), dataSize(%d)\n", clientIndex_, size_);

		auto pUser = m_pUserManager->GetUserByConnIdx(clientIndex_);
		pUser->SetPacketData(size_, pData_);
	}

	void Run(const UINT32 maxClient)
	{
		m_pPacketManager = std::make_unique<PacketManager>();
		m_pUserManager = std::make_unique<UserManager>();
		m_pRoomManager = std::make_unique<RoomManager>();

		auto sendPacketFunc = [&](UINT32 connectionIndex, UINT16 packetSize, char* pSendPacket)
		{
			SendMsg(connectionIndex, packetSize, pSendPacket);
		};

		m_pPacketManager->SendPacketFunc = sendPacketFunc;
		m_pPacketManager->Init(m_pUserManager.get(), m_pRoomManager.get());

		m_pUserManager->Init(maxClient);

		UINT32 startRoomNummber = 0;
		UINT32 maxRoomCount = 10;
		UINT32 maxRoomUserCount = 4;
		m_pRoomManager->SendPacketFunc = sendPacketFunc;

		m_pRoomManager->Init(startRoomNummber, maxRoomCount, maxRoomUserCount);

		m_pPacketManager->Run();
		

		StartServer(maxClient);
	}

	void End()
	{
		m_pPacketManager->End();
		
		DestroyThread();
	}


private:	
	std::unique_ptr<UserManager> m_pUserManager;
	std::unique_ptr<PacketManager> m_pPacketManager;
	std::unique_ptr<RoomManager> m_pRoomManager;
};