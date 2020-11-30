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
		mIndex = index;
		mPakcetDataBuffer = new char[PACKET_DATA_BUFFER_SIZE];
	}

	void Clear()
	{
		mRoomIndex = -1;
		mUserID = "";
		mIsConfirm = false;
		mCurDomainState = DOMAIN_STATE::NONE;

		mPakcetDataBufferWPos = 0;
		mPakcetDataBufferRPos = 0;
	}

	int SetLogin(char* userID_)
	{
		mCurDomainState = DOMAIN_STATE::LOGIN;
		mUserID = userID_;

		return 0;
	}
		
	void EnterRoom(INT32 roomIndex_)
	{
		mRoomIndex = roomIndex_;
		mCurDomainState = DOMAIN_STATE::ROOM;
	}
		
	void SetDomainState(DOMAIN_STATE value_) { mCurDomainState = value_; }

	INT32 GetCurrentRoom() 
	{
		return mRoomIndex;
	}

	INT32 GetNetConnIdx() 
	{
		return mIndex;
	}

	std::string GetUserId() const
	{
		return  mUserID;
	}

	DOMAIN_STATE GetDomainState() 
	{
		return mCurDomainState;
	}
		
	//TODO SetPacketData, GetPacket 함수를 멀티스레드에 호출하고 있다면 공유변수에 lock을 걸어야 한다
	void SetPacketData(const UINT32 dataSize_, char* pData_)
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

		CopyMemory(&mPakcetDataBuffer[mPakcetDataBufferWPos], pData_, dataSize_);
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
	INT32 mIndex = -1;

	INT32 mRoomIndex = -1;

	std::string mUserID;
	bool mIsConfirm = false;
	std::string mAuthToken;
	
	DOMAIN_STATE mCurDomainState = DOMAIN_STATE::NONE;		

	UINT32 mPakcetDataBufferWPos = 0;
	UINT32 mPakcetDataBufferRPos = 0;
	char* mPakcetDataBuffer = nullptr;
};

