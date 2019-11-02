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
			mPakcetDataBufferWPos = 0;
		}

		CopyMemory(&mPakcetDataBuffer[mPakcetDataBufferWPos], pData, dataSize_);
		mPakcetDataBufferWPos += dataSize_;
	}

	PacketInfo GetPacket()
	{
		const int PACKET_HEADER_LENGTH = 5;
		const int PACKET_SIZE_LENGTH = 2;
		const int PACKET_TYPE_LENGTH = 2;
		short packetSize = 0;

		while (true)
		{
			if (remainByte < PACKET_HEADER_LENGTH)
			{
				break;
			}

			CopyMemory(&packetSize, pBuffer, PACKET_SIZE_LENGTH);
			auto currentSize = packetSize;

			if (0 >= packetSize || packetSize > pConnection->RecvBufferSize())
			{
				char logmsg[128] = { 0, }; sprintf_s(logmsg, "IOCPServer::DoRecv. Arrived Wrong Packet.");
				LogFuncPtr((int)LogLevel::Error, logmsg);

				if (pConnection->CloseComplete())
				{
					HandleExceptionCloseConnection(pConnection);
				}
				return;
			}

			if (remainByte >= (DWORD)currentSize)
			{
				auto pMsg = m_pMsgPool->AllocMsg();
				if (pMsg == nullptr)
				{
					return;
				}

				pMsg->SetMessage(MessageType::OnRecv, pBuffer);
				if (PostNetMessage(pConnection, pMsg, currentSize) != NetResult::Success)
				{
					m_pMsgPool->DeallocMsg(pMsg);
					return;
				}

				remainByte -= currentSize;
				pBuffer += currentSize;
			}
			else
			{
				break;
			}
		}
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

