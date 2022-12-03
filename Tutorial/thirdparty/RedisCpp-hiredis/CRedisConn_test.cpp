/**
 * Copyright (c) 2015, 爱wifi（版权声明）
 *
 * @file	RedisConn_test.cpp		
 * @brief 此文件用于对　RedisConn 类进行测试。
 *
 * 此文件用于对　RedisConn 类进行测试。
 *
 * @author: 		yuhaiyang
 * @date: 		2015年4月30日
 *
 * 修订说明:初始版本
 */
#include "CRedisConn.h"
#include <unistd.h>

using  RedisCpp::CRedisConn;
using  RedisCpp::INSERT_POS;

/**
*-----测试过的内容-----
*每种方法不同情况下(if else)的错误提示、内存泄露
*connect auth reconnect
*/

void TestList( )
{

	int64_t ret = 0;
	uint64_t ret2 = 0;
	CRedisConn con;
	std::string value;

	if ( !con.connect( "127.0.0.1", 6379 ) )
	{
		std::cout << "connect error " << con.getErrorStr( ) << std::endl;
		return ;
	}

	if ( !con.auth("111") )
	{
		std::cout << "auth error: " << con.getErrorStr() << std::endl;
	}
	else
	{
		std::cout << "auth success" << std::endl;
	}


//	//< key不是list类型;  key不存在；value 已经存在； value不存在；正常情况
//	if ( !con.lpush( "test", "yuhaiyang", ret2 ) )
//	{
//		std::cout << "lpush error: " << con.getErrorStr( ) << std::endl;
//	}else
//	{
//		std::cout << "lpush len = " << ret2 << std::endl;
//	}


//	//< key 不存在 ；key 不是list类型；正常情况
//	if( !con.lpop( "test", value ) )
//	{
//		std::cout << "lpop error: " << con.getErrorStr( ) << std::endl;
//	}else
//	{
//		std::cout << "value: " << value << std::endl;
//	}


//	//< key 不是list类型；  key不存在；value 已经存在； value不存在；正常情况
//	if ( !con.rpush( "testM", "yuhaiyang", ret2 ) )
//	{
//		std::cout << "rpush error: " << con.getErrorStr( ) << std::endl;
//	}else
//	{
//		std::cout << "rpush len = " << ret2 << std::endl;
//	}
//
//
//	//< key 不存在 ；key 不是list类型; 正常情况
//	if( !con.rpop( "Atest", value ) )
//	{
//		std::cout << "rpop error: " << con.getErrorStr( ) << std::endl;
//	}else
//	{
//		std::cout << "value: " << value << std::endl;
//	}

//	//< key 不存在; key 不是list类型； 正常情况
//	if( !con.llen("test",ret2) )
//	{
//		std::cout << "llen error: " << con.getErrorStr( ) << std::endl;
//
//	}
//	else
//	{
//		std::cout << "llen = " << ret2 << std::endl;
//	}
//
//	//< key 不存在；key 不是list类型; pivot 不存在; 正常情况
//	if( !con.linsert("Ltest", RedisCpp::BEFORE, "value0", "chenjun", ret ) )
//	{
//		std::cout << "linsert error: " << con.getErrorStr( ) << std::endl;
//	}
//	else
//	{
//		std::cout << "revalue = " << ret << std::endl;
//	}
//
//
//
//	//< key不存在； key 不是list类型； start stop超出范围；正常情况
//	RedisCpp::ValueList valueList;
//	if( !con.lrange( "test",  0, -1, valueList ) )
//	{
//		std::cout << "lrange error: " << con.getErrorStr() << std::endl;
//	}else
//	{
//		RedisCpp::ValueList::const_iterator it = valueList.begin();
//
//		for( ; it != valueList.end(); it++ )
//		{
//			std::cout << "value: " << *it << std::endl;
//		}
//	}
//
//
	//key 不存在;  key 不是list类型; index不在范围内； 正常情况
	if( !con.lindex("test",1,value) )
	{
		std::cout << "lindex error: " << con.getErrorStr( ) << std::endl;
	}
	else
	{
		std::cout<<value.c_str()<<std::endl;
	}

}

void TestHash( void )
{

	int64_t ret = 0;
	uint64_t ret2 = 0;
	uint32_t ret3;
	RedisCpp::ValueMap valueMap;
	std::string value;
	CRedisConn con;
	if ( !con.connect( "127.0.0.1", 6379 ) )
	{
		std::cout << "connect error-- " << con.getErrorStr( ) << std::endl;
		return ;
	}

	if ( !con.auth("111") )
	{
		std::cout << "auth error: " << con.getErrorStr() << std::endl;
	}
	else
	{
		std::cout << "auth success" << std::endl;
	}


//	//<  key不是hash类型； key不存在； field不存在；正常情况
//	if ( !con.hget( "car", "color", value ) )
//	{
//		std::cout<<"___"<<std::endl;
//		std::cout << "hget error: " << con.getErrorStr( ) << std::endl;
//	}
//	else
//	{
//		std::cout << "hash value = " << value.c_str() << std::endl;
//	}
//
//	//< key 不存在；key 不是hash类型；field 不存在； field 已存在； 正常情况
//
//	if ( !con.hset( "test", "color", "bue", ret3 ) )
//	{
//		std::cout << "hget error: " << con.getErrorStr( ) << std::endl;
//	}
//	else
//	{
//		if ( ret3 == 1)
//		{
//			std::cout << "ret = 1; set a new value " << std::endl;
//		}
//		else if(ret3 == 0)
//		{
//			std::cout << "ret = 0; recovered value " << std::endl;
//		}
//
//	}


	//< key不存在； key不是hash类型； field不存在； field存在
	//< while循环时kill: _errStr = NULL pointer
//	while(1)
//	{
//		try
//		{
//			if ( !con.hdel( "car", "color",ret3 ) )
//			{
//				std::cout << "hdel error: " << con.getErrorStr() << std::endl;
//			}
//			else
//			{
//				std::cout << "ret3 = " << ret3 << std::endl;
//			}
//		}
//		catch(...)
//		{
//			//std::cout << "e.what:" << e.what() << std::endl;
//			std::cout << "_errStr" << con.getErrorStr() << std::endl;
//		}
//		sleep(3);
//	}

//	 key 不存在； key不是hash类型； key是hash类型
//	 for循环中 kill redis-server
	RedisCpp::ValueMap::iterator it;
//
//	for ( int i = 0; i < 5; i++)
//	{
//		try
//		{
//			if ( !con.hgetall( "bike", valueMap ) )
//			{
//				std::cout << "hgetall error: " << con.getErrorStr() << std::endl;
//			}
//			else
//			{
//				std::cout << "hgetall :" << std::endl;
//				for(it = valueMap.begin();it != valueMap.end(); ++it)
//				std::cout << "field: " << it->first << " value: " << it->second << std::endl;
//			}
//		}
//		catch(...)
//		{
//
//			std::cout << "_errStr:" << con.getErrorStr() << std::endl;
//			sleep(10);
//			std::cout << "start redis server?"<<std::endl;
//			if( !con.reconnect() )
//			{
//				std::cout << "reconn_errStr:" << con.getErrorStr() << std::endl;
//			}
//		}
//		sleep(1);
//		std::cout << "--------------------------------" << std::endl;
//	}

}

void Test( void )
{
	CRedisConn con;
	RedisCpp::ValueMap::iterator it;
	RedisCpp::ValueMap valueMap;
	RedisCpp::ValueList valueList;
	std::string value;

	int64_t ret = 0;
	uint64_t ret2 = 0;
	uint32_t ret3;
	//try
	//{
		while(1)
		{	//< host,port错误；redis-server未开启;
			if ( !con.connect( "127.0.0.1", 6379,"",0 ) )
			{
				std::cout << "connect error: " << con.getErrorStr( ) << std::endl;
				sleep(1);
				continue ;
			}
			break;
		}
		while ( 1 )
		{	std::cout << "...." << std::endl;
			//sleep(5);
			//< 密码错误；连接断开后认证；重复执行；
			if ( !con.auth("111") )
			{
				std::cout << "auth error: " << con.getErrorStr() << std::endl;
				//sleep(10);
				continue;
			}
			else
			{
				std::cout << "auth success" << std::endl;
				break;

			}
		}

		for(int j = 0; j < 1000; j++)
		{
			///////////////////////////hash////////////////////////////////////
			std::cout << "hgetll..............." << std::endl << std::endl;
			if ( !con.hgetall( "car", valueMap ) )
			{
				std::cout << "hgetall error: " << con.getErrorStr() << std::endl;
			}
			else
			{
				std::cout << "hgetall :" << std::endl;
				for(it = valueMap.begin(); it != valueMap.end(); ++it)
				std::cout << "field: " << it->first << " value: " << it->second << std::endl;
			}


			std::cout << "hget..............." << std::endl << std::endl;
			if ( !con.hget( "carC", "color", value ) )
			{
				std::cout << "hget error: " << con.getErrorStr( ) << std::endl;
			}
			else
			{
				std::cout << "hash value = " << value.c_str() << std::endl;
			}

			std::cout << "hset..............." << std::endl << std::endl;
			if ( !con.hset( "test", "colddddr", "bue", ret3 ) )
			{
				std::cout << "hget error: " << con.getErrorStr( ) << std::endl;
			}
			else
			{
				if ( ret3 == 1)
				{
					std::cout << " ret = 1; set a new value " << std::endl;
				}
				else if(ret3 == 0)
				{
					std::cout << "ret = 0; recovered value " << std::endl;
				}

			}

			std::cout << "hdel..............." << std::endl << std::endl;
			if ( !con.hdel("car", "color", ret3) )
			{
				std::cout << "hdel error: " << con.getErrorStr() << std::endl;
			}
			else
			{
				std::cout << "ret3 = " << ret3 << std::endl;
			}


			std::cout << "_errStr" << con.getErrorStr() << std::endl;


			///////////////////////////list////////////////////////////////////

			std::cout << "lpush..............." << std::endl << std::endl;
			if ( !con.lpush( "Ltest", "yuhaiyang", ret2 ) )
			{
				std::cout << "lpush error: " << con.getErrorStr( ) << std::endl;
			}else
			{
				std::cout << "len = " << ret2 << std::endl;
			}


			std::cout << "lpop..............." << std::endl << std::endl;
			if( !con.lpop( "Ltest", value ) )
			{
				std::cout << "lpop error: " << con.getErrorStr( ) << std::endl;
			}else
			{
				std::cout << "value: " << value << std::endl;
			}


			std::cout << "rpush..............." << std::endl << std::endl;
			if ( !con.rpush( "LtestM", "yuhaiyang", ret2 ) )
			{
				std::cout << "Lrpush error: " << con.getErrorStr( ) << std::endl;
			}else
			{
				std::cout << "Llen = " << ret2 << std::endl;
			}


			std::cout << "rpop..............." << std::endl << std::endl;
			if( !con.rpop( "LAtest", value ) )
			{
				std::cout << "rpop error: " << con.getErrorStr( ) << std::endl;
			}else
			{
				std::cout << "value: " << value << std::endl;
			}

			std::cout << "llen..............." << std::endl << std::endl;
			if( !con.llen("Ltest",ret2) )
			{
				std::cout << "llen error: " << con.getErrorStr( ) << std::endl;

			}
			else
			{
				std::cout << "llen = " << ret2 << std::endl;
			}

			std::cout << "linsert..............." << std::endl << std::endl;
			if( !con.linsert("Ltest", RedisCpp::BEFORE, "value0", "chenjun", ret ) )
			{
				std::cout << "linsert error: " << con.getErrorStr( ) << std::endl;
			}
			else
			{
				std::cout << "revalue = " << ret << std::endl;
			}




			std::cout << "lrange..............." << std::endl << std::endl;
			if( !con.lrange( "Ltest",  0, 2, valueList ) )
			{
				std::cout << "error " << con.getErrorStr() << std::endl;
			}else
			{
				RedisCpp::ValueList::const_iterator it = valueList.begin();

				for( ; it != valueList.end(); it++ )
				{
					std::cout << "value: " << *it << std::endl;
				}
			}


			std::cout << "lindex..............." << std::endl << std::endl;
			if( !con.lindex("Ltest",1,value) )
			{
				std::cout << "lindex error: " << con.getErrorStr( ) << std::endl;
			}
			else
			{
				std::cout<<value.c_str()<<std::endl;
			}

			//sleep(1);
		}
}


int main( )
{
//	TestList();
	TestHash();
//	Test();
	std::cout << "done main" << std::endl;
	return 0;
}


