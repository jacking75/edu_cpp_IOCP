#pragma once
#include <string>

#include "Packet.h"

class User
{
	const UINT32 PACKET_DATA_BUFFER_SIZE = 8096;

public:
	enum class DOMAIN_STATE 
	{
		NONE = 0,
		LOGIN = 1,
		ROOM = 2
	};


	User() = default;
	~User() = default;

	void Init(const INT32 index)
	{
		m_Index = index;
		mPakcetDataBuffer = new char[PACKET_DATA_BUFFER_SIZE];
	}

	void Clear()
	{
		m_RoomIndex = -1;
		m_UserId = "";
		m_IsConfirm = false;
		m_CurDomainState = DOMAIN_STATE::NONE;

		mPakcetDataBufferWPos = 0;
		mPakcetDataBufferRPos = 0;
	}

	int SetLogin(char* login_id)
	{
		m_CurDomainState = DOMAIN_STATE::LOGIN;
		m_UserId = login_id;

		return 0;
	}
		
	void EnterRoom(INT32 roomIndex)
	{
		m_RoomIndex = roomIndex;
		m_CurDomainState = DOMAIN_STATE::ROOM;
	}
		
	void SetDomainState(DOMAIN_STATE val) { m_CurDomainState = val; }

	INT32 GetCurrentRoom() 
	{
		return m_RoomIndex;
	}

	INT32 GetNetConnIdx() 
	{
		return m_Index;
	}

	std::string GetUserId() const
	{
		return  m_UserId;
	}

	DOMAIN_STATE GetDomainState() 
	{
		return m_CurDomainState;
	}
				
	void SetPacketData(const UINT32 dataSize_, char* pData)
	{
		if ((mPakcetDataBufferWPos + dataSize_) >= PACKET_DATA_BUFFER_SIZE)
		{
			auto remainDataSize = mPakcetDataBufferWPos - mPakcetDataBufferRPos;

			if (remainDataSize > 0)
			{
				CopyMemory(&mPakcetDataBuffer[0], &mPakcetDataBuffer[mPakcetDataBufferRPos], remainDataSize);
				mPakcetDataBufferWPos = remainDataSize;
			}
			else
			{
				mPakcetDataBufferWPos = 0;
			}
			
			mPakcetDataBufferRPos = 0;
		}

		CopyMemory(&mPakcetDataBuffer[mPakcetDataBufferWPos], pData, dataSize_);
		mPakcetDataBufferWPos += dataSize_;
	}

	PacketInfo GetPacket()
	{
		const int PACKET_SIZE_LENGTH = 2;
		const int PACKET_TYPE_LENGTH = 2;
		short packetSize = 0;
		
		UINT32 remainByte = mPakcetDataBufferWPos - mPakcetDataBufferRPos;

		if(remainByte < PACKET_HEADER_LENGTH)
		{
			return PacketInfo();
		}

		auto pHeader = (PACKET_HEADER*)&mPakcetDataBuffer[mPakcetDataBufferRPos];
		
		if (pHeader->PacketLength > remainByte)
		{
			return PacketInfo();
		}

		PacketInfo packetInfo;
		packetInfo.PacketId = pHeader->PacketId;
		packetInfo.DataSize = pHeader->PacketLength;
		packetInfo.pDataPtr = &mPakcetDataBuffer[mPakcetDataBufferRPos];
		
		mPakcetDataBufferRPos += pHeader->PacketLength;

		return packetInfo;
	}


private:
	INT32 m_RoomIndex = -1;

	std::string m_UserId;
	bool m_IsConfirm = false;
	std::string AuthToken;

	INT32 m_Index = -1;

	DOMAIN_STATE m_CurDomainState = DOMAIN_STATE::NONE;		

	UINT32 mPakcetDataBufferWPos = 0;
	UINT32 mPakcetDataBufferRPos = 0;
	char* mPakcetDataBuffer = nullptr;
};

