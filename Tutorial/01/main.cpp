#include "IOCompletionPort.h"

const INT16 SERVER_PORT = 32452;

int main()
{
	IOCompletionPort ioCompletionPort;

	//소켓을 초기화
	ioCompletionPort.InitSocket();

	//소켓과 서버 주소를 연결하고 등록 시킨다.
	ioCompletionPort.BindandListen(SERVER_PORT);

	ioCompletionPort.StartServer();

	printf("아무 키나 누를 때까지 대기합니다\n");
	getchar();
	ioCompletionPort.DestroyThread();
	return 0;
}

