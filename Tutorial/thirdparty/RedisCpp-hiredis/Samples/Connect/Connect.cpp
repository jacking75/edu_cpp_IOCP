// Connect.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>

#include "../../src/CRedisConn.h"

int main()
{
	//int64_t ret = 0;
	//uint64_t ret2 = 0;

	RedisCpp::CRedisConn con;
	
	std::string value;

	if (!con.connect("127.0.0.1", 6379))
	{
		std::cout << "connect error " << con.getErrorStr() << std::endl;
		return -1;
	}
	else
	{
		std::cout << "connect success !!!" << std::endl;
	}

	return 0;
}

