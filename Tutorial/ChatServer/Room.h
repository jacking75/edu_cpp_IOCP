#pragma once

#include "UserManager.h"
#include "Packet.h"

#include <functional>

#define MAX_USER_ID_LEN  20

class Room 
{
public:
	Room() = default;
	~Room() = default;

	INT32 GetMaxUserCount() { return m_MaxUserCount; }

	INT32 GetCurrentUserCount() { return m_CurrentUserCount; }

	INT32 GetRoomNumber() { return m_RoomNum; }

	
	void Init(const INT32 roomNum, const INT32 maxUserCount)
	{
		m_RoomNum = roomNum;
		m_MaxUserCount = maxUserCount;
	}

	UINT16 EnterUser(User* pUser)
	{
		if (m_CurrentUserCount >= m_MaxUserCount)
		{
			return (UINT16)ERROR_CODE::ENTER_ROOM_FULL_USER;
		}

		m_UserList.push_back(pUser);
		++m_CurrentUserCount;

		pUser->EnterRoom(m_RoomNum);
		return (UINT16)ERROR_CODE::NONE;
	}
		
	void LeaveUser(User* pLeaveUser)
	{
		m_UserList.remove_if([leaveUserId = pLeaveUser->GetUserId()](User* pUser) {
			return leaveUserId == pUser->GetUserId();
		});
	}
						
	void NotifyChat(INT32 connIndex, const char* UserID, const char* Msg)
	{
		ROOM_CHAT_NOTIFY_PACKET roomChatNtfyPkt;
		roomChatNtfyPkt.PacketId = (UINT16)PACKET_ID::ROOM_CHAT_NOTIFY;
		roomChatNtfyPkt.PacketLength = sizeof(roomChatNtfyPkt);

		CopyMemory(roomChatNtfyPkt.Msg, Msg, sizeof(roomChatNtfyPkt.Msg));
		CopyMemory(roomChatNtfyPkt.UserID, UserID, sizeof(roomChatNtfyPkt.UserID));
		SendToAllUser(sizeof(roomChatNtfyPkt), &roomChatNtfyPkt, connIndex, false);
	}
		
		
	std::function<void(UINT32, UINT32, char*)> SendPacketFunc;

private:
	void SendToAllUser(const UINT16 dataSize, void* pData, const INT32 passUserindex, bool exceptMe)
	{
		for (auto pUser : m_UserList)
		{
			if (pUser == nullptr) {
				continue;
			}

			if (exceptMe && pUser->GetNetConnIdx() == passUserindex) {
				continue;
			}

			SendPacketFunc((UINT32)pUser->GetNetConnIdx(), (UINT32)dataSize, pData);
		}
	}


	INT32 m_RoomNum = -1;

	std::list<User*> m_UserList;
		
	INT32 m_MaxUserCount = 0;

	UINT16 m_CurrentUserCount = 0;
};
