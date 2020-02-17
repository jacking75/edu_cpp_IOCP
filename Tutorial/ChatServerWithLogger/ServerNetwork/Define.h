#pragma once

#include <plog/Log.h>
#include<plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/EventLogAppender.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>

const UINT32 MAX_SOCK_RECVBUF = 256;	// 소켓 버퍼의 크기
const UINT32 MAX_SOCK_SENDBUF = 4096;	// 소켓 버퍼의 크기
const UINT64 RE_USE_SESSION_WAIT_TIMESEC = 3;


enum class IOOperation
{
	ACCEPT,
	RECV,
	SEND
};

//WSAOVERLAPPED구조체를 확장 시켜서 필요한 정보를 더 넣었다.
struct stOverlappedEx
{
	WSAOVERLAPPED m_wsaOverlapped;		//Overlapped I/O구조체
	WSABUF		m_wsaBuf;				//Overlapped I/O작업 버퍼
	IOOperation m_eOperation;			//작업 동작 종류
	UINT32 SessionIndex = 0;
};

