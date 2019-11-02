#pragma once
#include <unordered_map>
#include <functional>

#include "RoomManager.h"


class PacketManager {
public:
	PacketManager() = default;
	~PacketManager() = default;

	void Init(UserManager* pUserManager, RoomManager* pRoomManager);

	void ProcessRecvPacket(const UINT32 connectionIndex_, const UINT16 packetId_, const UINT16 packetSize_, char* pPacket_);

	void ProcessLogin(UINT32 connIndex, UINT16 packetSize_, char* pPacket_);
	void ProcessEnterRoom(UINT32 connIndex, UINT16 packetSize_, char* pPacket_);
	void ProcessLeaveRoom(UINT32 connIndex, UINT16 packetSize_, char* pPacket_);
	void ProcessRoomChatMessage(UINT32 connIndex, UINT16 packetSize_, char* pPacket_);
		

	void ClearConnectionInfo(INT32 connIndex);
		

	std::function<void(UINT32, UINT32, char*)> SendPacketFunc;


private:
	typedef void(PacketManager::* PROCESS_RECV_PACKET_FUNCTION)(UINT32, UINT16, char*);
	std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> m_RecvFuntionDictionary;

	UserManager* m_pUserManager;
	RoomManager* m_pRoomManager;
		
	std::function<void(int, char*)> m_SendMQDataFunc;
};

