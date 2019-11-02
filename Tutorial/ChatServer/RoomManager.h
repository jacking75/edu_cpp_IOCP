#pragma once
#include "Room.h"

class RoomManager
{
public:
	RoomManager() = default;
	~RoomManager() = default;

	void Init(const INT32 beginRoomNumber, const INT32 maxRoomCount, const INT32 maxRoomUserCount)
	{
		m_BeginRoomNumber = beginRoomNumber;
		m_MaxRoomCount = maxRoomCount;
		m_EndRoomNumber = beginRoomNumber + maxRoomCount;

		m_RoomList = std::vector<Room*>(maxRoomCount);

		for (auto i = 0; i < maxRoomCount; i++)
		{
			m_RoomList[i] = new Room();
			m_RoomList[i]->SendPacketFunc = SendPacketFunc;
			m_RoomList[i]->Init((i+ beginRoomNumber), maxRoomUserCount);
		}
	}

	UINT GetMaxRoomCount() { return m_MaxRoomCount; }
		
	UINT16 EnterUser(INT32 roomNumber, User* pUser)
	{
		auto pRoom = GetRoomByNumber(roomNumber);
		if (pRoom == nullptr)
		{
			return (UINT16)ERROR_CODE::ROOM_INVALID_INDEX;
		}


		return pRoom->EnterUser(pUser);
	}
		
	INT16 LeaveUser(INT32 roomNumber, User* pUser)
	{
		auto pRoom = GetRoomByNumber(roomNumber);
		if (pRoom == nullptr)
		{
			return (INT16)ERROR_CODE::ROOM_INVALID_INDEX;
		}
			
		pUser->SetDomainState(User::DOMAIN_STATE::LOGIN);
		pRoom->LeaveUser(pUser);
		return (INT16)ERROR_CODE::NONE;
	}

	Room* GetRoomByNumber(INT32 number) 
	{ 
		if (number < m_BeginRoomNumber || number >= m_EndRoomNumber)
		{
			return nullptr;
		}

		auto index = (number - m_BeginRoomNumber);
		return m_RoomList[index]; 
	} 

		
	std::function<void(UINT32, UINT16, char*)> SendPacketFunc;
		

private:
	std::vector<Room*> m_RoomList;
	INT32 m_BeginRoomNumber = 0;
	INT32 m_EndRoomNumber = 0;
	INT32 m_MaxRoomCount = 0;
};
