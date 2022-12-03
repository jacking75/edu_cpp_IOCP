#include <iostream>

#include "../../src/CRedisConn.h"

void TestList( )
{
	int64_t ret = 0;
	uint64_t ret2 = 0;
	RedisCpp::CRedisConn con;
	std::string value;

	if ( !con.connect( "127.0.0.1", 6379 ) )
	{
		std::cout << "connect error " << con.getErrorStr( ) << std::endl;
		return; 
	}

	/*if ( !con.auth("111") )
	{
		std::cout << "auth error: " << con.getErrorStr() << std::endl;
	}
	else
	{
		std::cout << "auth success" << std::endl;
	}*/

	std::cout << "lpush-lpop" << std::endl;
	// lpush
	if ( !con.lpush( "test", "yuhaiyang", ret2 ) )
	{
		std::cout << "lpush error: " << con.getErrorStr( ) << std::endl;
	}else
	{
		std::cout << "lpush len = " << ret2 << std::endl;
	}

	//LINDEX  https://kwoncharlie.blog.me/220397447626
	if (!con.lindex("test", 1, value))
	{
		std::cout << "lindex error: " << con.getErrorStr() << std::endl;
	}
	else
	{
		std::cout << value.c_str() << std::endl;
	}

	//lpop
	if( !con.lpop( "test", value ) )
	{
		std::cout << "lpop error: " << con.getErrorStr( ) << std::endl;
	}else
	{
		std::cout << "value: " << value << std::endl;
	}
	std::cout << std::endl;


	std::cout << "rpush-rpop" << std::endl;
	//rpush
	if ( !con.rpush( "testM", "yuhaiyang", ret2 ) )
	{
		std::cout << "rpush error: " << con.getErrorStr( ) << std::endl;
	}else
	{
		std::cout << "rpush len = " << ret2 << std::endl;
	}


	//rpop	
	if( !con.rpop( "Atest", value ) )
	{
		std::cout << "rpop error: " << con.getErrorStr( ) << std::endl;
	}else
	{
		std::cout << "value: " << value << std::endl;
	}
	std::cout << std::endl;

	
	std::cout << "llen" << std::endl;
	if( !con.llen("test",ret2) )
	{
		std::cout << "llen error: " << con.getErrorStr( ) << std::endl;

	}
	else
	{
		std::cout << "llen = " << ret2 << std::endl;
	}
	std::cout << std::endl;


	std::cout << "linsert" << std::endl;
	if( !con.linsert("Ltest", RedisCpp::BEFORE, "value0", "chenjun", ret ) )
	{
		std::cout << "linsert error: " << con.getErrorStr( ) << std::endl;
	}
	else
	{
		std::cout << "revalue = " << ret << std::endl;
	}
	std::cout << std::endl;



	std::cout << "lrange" << std::endl;
	RedisCpp::ValueList valueList;
	if( !con.lrange( "test",  0, -1, valueList ) )
	{
		std::cout << "lrange error: " << con.getErrorStr() << std::endl;
	}else
	{
		RedisCpp::ValueList::const_iterator it = valueList.begin();

		for( ; it != valueList.end(); it++ )
		{
			std::cout << "value: " << *it << std::endl;
		}
	}

}

int main(int argc, char* argv[])
{
	TestList();
	return 0;
}