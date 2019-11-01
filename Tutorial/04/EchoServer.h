#pragma once

#include "IOCPServer.h"
#include "Packet.h"

#include <vector>
#include <thread>
#include <deque>

class EchoServer : public IOCPServer
{
	EchoServer() = default;
	virtual ~EchoServer() = default;


public:
	virtual void OnConnect(const UINT32 clientIndex_) override 
	{
		printf("[OnConnect] 클라이언트: Index(%d)\n", clientIndex_);
	}

	virtual void OnClose(const UINT32 clientIndex_) override 
	{
		printf("[OnClose] 클라이언트: Index(%d)\n", clientIndex_);
	}

	virtual void OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) override  
	{
		printf("[OnReceive] 클라이언트: Index(%d), dataSize(%d)\n", clientIndex_, size_);
	}

	void Run()
	{
		mIsRunProcessThread = true;
	}

	void End()
	{
		mIsRunProcessThread = false;

		if (mProcessThread.joinable())
		{
			mProcessThread.join();
		}
	}

private:
	void ProcessPacket()
	{
		while (mIsRunProcessThread)
		{

		}
	}

	bool mIsRunProcessThread = false;

	std::thread mProcessThread;

	std::deque<PacketData> mPacketDataQueue;
};