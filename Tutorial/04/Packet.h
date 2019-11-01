#pragma once

struct PacketData
{
	unsigned int SessionIndex = 0;
	int DataSize = 0;
	char* pPacketData = nullptr;

	void Release()
	{
		delete pPacketData;
	}
};