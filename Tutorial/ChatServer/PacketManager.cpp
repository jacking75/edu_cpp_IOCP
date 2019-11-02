#include <utility>
#include <cstring>


#include "UserManager.h"
#include "RoomManager.h"
#include "PacketManager.h"
#include "RedisManager.h"


void PacketManager::Init(const UINT32 maxClient)
{
	m_RecvFuntionDictionary = std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION>();

	m_RecvFuntionDictionary[(int)PACKET_ID::SYS_USER_CONNECT] = &PacketManager::ProcessUserConnect;
	m_RecvFuntionDictionary[(int)PACKET_ID::SYS_USER_DISCONNECT] = &PacketManager::ProcessUserDisConnect;

	m_RecvFuntionDictionary[(int)PACKET_ID::LOGIN_REQUEST] = &PacketManager::ProcessLogin;
	
	m_RecvFuntionDictionary[(int)PACKET_ID::ROOM_ENTER_REQUEST] = &PacketManager::ProcessEnterRoom;
	m_RecvFuntionDictionary[(int)PACKET_ID::ROOM_LEAVE_REQUEST] = &PacketManager::ProcessLeaveRoom;
	m_RecvFuntionDictionary[(int)PACKET_ID::ROOM_CHAT_REQUEST] = &PacketManager::ProcessRoomChatMessage;
				
	CreateCompent(maxClient);

	mRedisMgr = new RedisManager;// std::make_unique<RedisManager>();
}

void PacketManager::CreateCompent(const UINT32 maxClient)
{
	m_pUserManager = new UserManager;
	m_pUserManager->Init(maxClient);

		
	UINT32 startRoomNummber = 0;
	UINT32 maxRoomCount = 10;
	UINT32 maxRoomUserCount = 4;
	m_pRoomManager = new RoomManager;
	m_pRoomManager->SendPacketFunc = SendPacketFunc;
	m_pRoomManager->Init(startRoomNummber, maxRoomCount, maxRoomUserCount);
}

bool PacketManager::Run()
{	
	if (mRedisMgr->Run("127.0.0.1", 6379, 1) == false)
	{
		return false;
	}

	//이 부분을 패킷 처리 부분으로 이동 시킨다.
	mIsRunProcessThread = true;
	mProcessThread = std::thread([this]() { ProcessPacket(); });

	return true;
}

void PacketManager::End()
{
	mRedisMgr->End();

	mIsRunProcessThread = false;

	if (mProcessThread.joinable())
	{
		mProcessThread.join();
	}
}

void PacketManager::ReceivePacketData(const UINT32 clientIndex_, const UINT32 size_, char* pData_)
{
	auto pUser = m_pUserManager->GetUserByConnIdx(clientIndex_);
	pUser->SetPacketData(size_, pData_);

	EnqueuePacketData(clientIndex_);
}

void PacketManager::ProcessPacket()
{
	while (mIsRunProcessThread)
	{
		bool isIdle = true;

		if (auto packetData = DequePacketData(); packetData.PacketId > (UINT16)PACKET_ID::SYS_END)
		{
			isIdle = false;
			ProcessRecvPacket(packetData.SessionIndex, packetData.PacketId, packetData.DataSize, packetData.pDataPtr);
		}

		if (auto packetData = DequeSystemPacketData(); packetData.PacketId != 0)
		{
			isIdle = false;
			ProcessRecvPacket(packetData.SessionIndex, packetData.PacketId, packetData.DataSize, packetData.pDataPtr);
		}

		if(isIdle)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

void PacketManager::EnqueuePacketData(const UINT32 sessionIndex_)
{
	std::lock_guard<std::mutex> guard(mLock);
	mInComingPacketUserIndex.push_back(sessionIndex_);
}

PacketInfo PacketManager::DequePacketData()
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

void PacketManager::PushSystemPacket(PacketInfo packet)
{
	std::lock_guard<std::mutex> guard(mLock);
	mSystemPacketQueue.push_back(packet);
}

PacketInfo PacketManager::DequeSystemPacketData()
{
	
	std::lock_guard<std::mutex> guard(mLock);
	if (mSystemPacketQueue.empty())
	{
		return PacketInfo();
	}

	auto packetData = mSystemPacketQueue.front();
	mSystemPacketQueue.pop_front();
	
	return packetData;
}

void PacketManager::ProcessRecvPacket(const UINT32 connectionIndex_, const UINT16 packetId_, const UINT16 packetSize_, char* pPacket_)
{
	auto iter = m_RecvFuntionDictionary.find(packetId_);
	if (iter != m_RecvFuntionDictionary.end())
	{
		(this->*(iter->second))(connectionIndex_, packetSize_, pPacket_);
	}

}


void PacketManager::ProcessUserConnect(UINT32 connIndex, UINT16 packetSize_, char* pPacket_)
{
	auto pUser = m_pUserManager->GetUserByConnIdx(connIndex);
	pUser->Clear();
}

void PacketManager::ProcessUserDisConnect(UINT32 connIndex, UINT16 packetSize_, char* pPacket_)
{
	ClearConnectionInfo(connIndex);
}

void PacketManager::ProcessLogin(UINT32 connIndex, UINT16 packetSize_, char* pPacket_)
{ 
	if (LOGIN_REQUEST_PACKET_SZIE != packetSize_)
	{
		return;
	}

	auto pLoginReqPacket = reinterpret_cast<LOGIN_REQUEST_PACKET*>(pPacket_);

	auto pUserID = pLoginReqPacket->UserID;
	printf("requested user id = %s\n", pUserID);

	LOGIN_RESPONSE_PACKET loginResPacket;
	loginResPacket.PacketId = (UINT16)PACKET_ID::LOGIN_RESPONSE;
	loginResPacket.PacketLength = sizeof(LOGIN_RESPONSE_PACKET);

	if (m_pUserManager->GetCurrentUserCnt() >= m_pUserManager->GetMaxUserCnt()) 
	{ 
		//접속자수가 최대수를 차지해서 접속불가
		loginResPacket.Result = (UINT16)ERROR_CODE::LOGIN_USER_USED_ALL_OBJ;
		SendPacketFunc(connIndex, sizeof(LOGIN_RESPONSE_PACKET) , (char*)&loginResPacket);
		return;
	}

	//여기에서 이미 접속된 유저인지 확인하고, 접속된 유저라면 실패한다.
	if (m_pUserManager->FindUserIndexByID(pUserID) == -1) 
	{ 
		RedisLoginReq dbReq;
		CopyMemory(dbReq.UserID, pLoginReqPacket->UserID, (MAX_USER_ID_LEN + 1));
		CopyMemory(dbReq.UserPW, pLoginReqPacket->UserPW, (MAX_USER_PW_LEN + 1));

		RedisTask task;
		task.UserIndex = connIndex;
		task.TaskID = RedisTaskID::REQUEST_LOGIN;
		task.DataSize = sizeof(RedisLoginReq);
		task.pData = new char[task.DataSize];
		mRedisMgr->PushTask(task);

		printf("Login To Redis user id = %s\n", pUserID);
	}
	else 
	{
		//접속중인 유저여서 실패를 반환한다.
		loginResPacket.Result = (UINT16)ERROR_CODE::LOGIN_USER_ALREADY;
		SendPacketFunc(connIndex, sizeof(LOGIN_RESPONSE_PACKET), (char*)&loginResPacket);
		return;
	}

	SendPacketFunc(connIndex, sizeof(LOGIN_RESPONSE_PACKET), (char*)&loginResPacket);
}



void PacketManager::ProcessEnterRoom(UINT32 connIndex, UINT16 packetSize_, char* pPacket_)
{
	UNREFERENCED_PARAMETER(packetSize_);

	auto pRoomEnterReqPacket = reinterpret_cast<ROOM_ENTER_REQUEST_PACKET*>(pPacket_);
	auto pReqUser = m_pUserManager->GetUserByConnIdx(connIndex);

	if (!pReqUser || pReqUser == nullptr) 
	{
		return;
	}
				
	ROOM_ENTER_RESPONSE_PACKET roomEnterResPacket;
	roomEnterResPacket.PacketId = (UINT16)PACKET_ID::ROOM_ENTER_RESPONSE;
	roomEnterResPacket.PacketLength = sizeof(ROOM_ENTER_RESPONSE_PACKET);
				
	roomEnterResPacket.Result = m_pRoomManager->EnterUser(pRoomEnterReqPacket->RoomNumber, pReqUser);

	SendPacketFunc(connIndex, sizeof(ROOM_ENTER_RESPONSE_PACKET), (char*)&roomEnterResPacket);
	printf("Response Packet Sended");
}


void PacketManager::ProcessLeaveRoom(UINT32 connIndex, UINT16 packetSize_, char* pPacket_)
{
	UNREFERENCED_PARAMETER(packetSize_);
	UNREFERENCED_PARAMETER(pPacket_);

	ROOM_LEAVE_RESPONSE_PACKET roomLeaveResPacket;
	roomLeaveResPacket.PacketId = (UINT16)PACKET_ID::ROOM_LEAVE_RESPONSE;
	roomLeaveResPacket.PacketLength = sizeof(ROOM_LEAVE_RESPONSE_PACKET);

	auto reqUser = m_pUserManager->GetUserByConnIdx(connIndex);
	auto roomNum = reqUser->GetCurrentRoom();
				
	//TODO Room안의 UserList객체의 값 확인하기
	roomLeaveResPacket.Result = m_pRoomManager->LeaveUser(roomNum, reqUser);
	SendPacketFunc(connIndex, sizeof(ROOM_LEAVE_RESPONSE_PACKET), (char*)&roomLeaveResPacket);
}


void PacketManager::ProcessRoomChatMessage(UINT32 connIndex, UINT16 packetSize_, char* pPacket_)
{
	UNREFERENCED_PARAMETER(packetSize_);

	auto pRoomChatReqPacketet = reinterpret_cast<ROOM_CHAT_REQUEST_PACKET*>(pPacket_);
		
	ROOM_CHAT_RESPONSE_PACKET roomChatResPacket;
	roomChatResPacket.PacketId = (UINT16)PACKET_ID::ROOM_CHAT_RESPONSE;
	roomChatResPacket.PacketLength = sizeof(ROOM_CHAT_RESPONSE_PACKET);
	roomChatResPacket.Result = (INT16)ERROR_CODE::NONE;

	auto reqUser = m_pUserManager->GetUserByConnIdx(connIndex);
	auto roomNum = reqUser->GetCurrentRoom();
				
	auto pRoom = m_pRoomManager->GetRoomByNumber(roomNum);
	if (pRoom == nullptr)
	{
		roomChatResPacket.Result = (INT16)ERROR_CODE::CHAT_ROOM_INVALID_ROOM_NUMBER;
		SendPacketFunc(connIndex, sizeof(ROOM_CHAT_RESPONSE_PACKET), (char*)&roomChatResPacket);
		return;
	}
		
	SendPacketFunc(connIndex, sizeof(ROOM_CHAT_RESPONSE_PACKET), (char*)&roomChatResPacket);

	pRoom->NotifyChat(connIndex, reqUser->GetUserId().c_str(), pRoomChatReqPacketet->Message);		
}		   

void PacketManager::ClearConnectionInfo(INT32 connIndex) 
{
	auto pReqUser = m_pUserManager->GetUserByConnIdx(connIndex);

	if (pReqUser->GetDomainState() == User::DOMAIN_STATE::ROOM) 
	{
		auto roomNum = pReqUser->GetCurrentRoom();
		m_pRoomManager->LeaveUser(roomNum, pReqUser);
	}

	if (pReqUser->GetDomainState() != User::DOMAIN_STATE::NONE) 
	{
		m_pUserManager->DeleteUserInfo(pReqUser);
	}
}
