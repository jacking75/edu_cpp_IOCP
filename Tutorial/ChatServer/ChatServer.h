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

//const UINT32 MAX_USER_COUNT = 
class ChatServer : public IOCPServer
{
public:
	ChatServer() = default;
	virtual ~ChatServer() = default;
	

	virtual void OnConnect(const UINT32 clientIndex_) override 
	{
		printf("[OnConnect] 클라이언트: Index(%d)\n", clientIndex_);

		//TODO User 객체 초기화 하기. 시스템 패킷을 보낸다.
	}

	virtual void OnClose(const UINT32 clientIndex_) override 
	{
		printf("[OnClose] 클라이언트: Index(%d)\n", clientIndex_);

		//TODO 종료 처리하기. 시스템 패킷을 보낸다.
	}

	virtual void OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) override  
	{
		printf("[OnReceive] 클라이언트: Index(%d), dataSize(%d)\n", clientIndex_, size_);

		auto pUser = m_pUserManager->GetUserByConnIdx(clientIndex_);
		pUser->SetPacketData(size_, pData_);

		std::lock_guard<std::mutex> guard(mLock);
		mInComingPacketUserIndex.push_back(clientIndex_);		
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

		//이 부분을 패킷 처리 부분으로 이동 시킨다.
		mIsRunProcessThread = true;
		mProcessThread = std::thread([this]() { ProcessPacket(); });

		StartServer(maxClient);
	}

	void End()
	{
		mIsRunProcessThread = false;

		if (mProcessThread.joinable())
		{
			mProcessThread.join();
		}

		DestroyThread();
	}

private:
	void ProcessPacket()
	{
		while (mIsRunProcessThread)
		{
			auto packetData = DequePacketData();
			if (packetData.PacketId != 0)
			{
				m_pPacketManager->ProcessRecvPacket(packetData.SessionIndex, packetData.PacketId, packetData.DataSize, packetData.pDataPtr);
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}

	PacketInfo DequePacketData()
	{
		UINT32 userIndex = 0;

		{
			std::lock_guard<std::mutex> guard(mLock);
			if (mInComingPacketUserIndex.empty())
			{
				return PacketInfo();
			}

			userIndex = mInComingPacketUserIndex.front();
			mInComingPacketUserIndex.pop_front();
		}

		auto pUser = m_pUserManager->GetUserByConnIdx(userIndex);
		auto packetData = pUser->GetPacket();		
		packetData.SessionIndex = userIndex;
		return packetData;
	}


	bool mIsRunProcessThread = false;

	std::thread mProcessThread;

	std::mutex mLock;
	std::deque<UINT32> mInComingPacketUserIndex;
	//char* mPacketDataBuffer = nullptr;

	std::unique_ptr<UserManager> m_pUserManager;
	std::unique_ptr<PacketManager> m_pPacketManager;
	std::unique_ptr<RoomManager> m_pRoomManager;
};