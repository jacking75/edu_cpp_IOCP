#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// 클라이언트가 보낸 패킷을 저장하는 구조체
struct PacketData
{
	UINT32 SessionIndex = 0;
	UINT32 DataSize = 0;
	char* pPacketData = nullptr;

	void Set(PacketData& vlaue)
	{
		SessionIndex = vlaue.SessionIndex;
		DataSize = vlaue.DataSize;

		pPacketData = new char[vlaue.DataSize];
		CopyMemory(pPacketData, vlaue.pPacketData, vlaue.DataSize);
	}

	void Set(UINT32 sessionIndex_, UINT32 dataSize_, char* pData)
	{
		SessionIndex = sessionIndex_;
		DataSize = dataSize_;

		pPacketData = new char[dataSize_];
		CopyMemory(pPacketData, pData, dataSize_);
	}

	void Release()
	{
		delete pPacketData;
	}
};