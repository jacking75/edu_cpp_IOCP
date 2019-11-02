#pragma once

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <stdexcept>

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "hiredis.lib")

#include <winsock.h>
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4200)
#include <hiredis.h>
#pragma warning(pop)
#else
#include <hiredis.h>
#endif

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN( typeName )	\
	typeName( const typeName & );		\
	typeName & operator=( const typeName & )
#endif

namespace RedisCpp
{

typedef std::list<std::string> ValueList;
typedef std::map<std::string , std::string> ValueMap;



///< 这个是插入时候是插入在指定元素之前，或者指定元素之后
/*typedef */enum INSERT_POS
{
	BEFORE,			///< 插入到指定元素之前。
	AFTER
} E_INSERT_POS;

/**
 *@brief 此类基于 hiredis 用于保持与 redis-server 的链接。
 * 
 * 例子代码:
 *int main( )
 {
 RedisCpp::RedisConn con;
 if ( !con.connect( "127.0.0.1", 6379, "521110", 5 ) )
 {
 std::cout << "connect error " << con.getErrorStr( ) << std::endl;
 return 0;
 }

 std::string value;
 if ( !con.hget( "newHash", "yuhaiyang", value ) )
 {
 std::cout << "hget error " << con.getErrorStr( ) << std::endl;
 }
 return 0;
 }
 *
 */
class CRedisConn
{
public:
	CRedisConn( void )
	{
		_redCtx = NULL;
		_host.clear();
		_password.clear();
		_port = 0;
		_timeout = 0;
		_connected = false;
		_errStr = _errDes[ERR_NO_ERROR];
	}

	virtual ~CRedisConn(void)
	{
		disConnect();
	}

	/**
	 *@brief 初始化链接信息
	 *
	 *用于初始化链接的详细信息。在　connect() 之前使用。也可以直接调用
	 *bool connect( const std::string &host ,const uint16_t port, const std::string& password , const uint32_t timeout );来初始化。
	 */
	void init( const std::string &host = "127.0.0.1" , const uint16_t port = 6379 ,
	                const std::string& password = "" , const uint32_t timeout = 0 )
	{
		_host = host;
		_port = port;
		_password = password;
		_timeout = timeout;
	}


	/**
	 *@brief 链接　redis 数据库
	 *
	 * init() 之后调用此方法来链接数据库
	 *
	 *@return 成功返回　true,失败返回　false
	 */
	bool connect()
	{
		if (_connected)
		{
			disConnect();
		}

		struct timeval timeoutVal;
		timeoutVal.tv_sec = _timeout;
		timeoutVal.tv_usec = 0;

		_redCtx = redisConnectWithTimeout(_host.c_str(), _port, timeoutVal);
		if (_getError(_redCtx))
		{
			if (NULL != _redCtx)
			{
				redisFree(_redCtx);
				_redCtx = NULL;
			}
			_connected = false;
			return false;
		}

		_connected = true;
		return true;

		// if connection  need password
	/*
		if ( _password == "" )
		{
			return true;
		}
		else
		{
			return ( auth( _password ) );   //< 授权失败也将返回false
		}
	*/
	}

	/**
	 *@brief 断开与　redis 内存数据库链接
	 */
	void disConnect( )
	{
		if (_connected && NULL != _redCtx)
		{
			redisFree(_redCtx);
			_redCtx = NULL;
		}
		_connected = false;
	}

	/**
	 *@brief 链接　redis 数据库之后，需要认证。
	 *
	 *@param [in] 登录的密码。
	 *@return 成功返回　true, 失败返回　false.
	 */
	bool auth( const std::string& password )
	{
		if (!_connected)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		_password = password;
		redisReply* reply = static_cast<redisReply*>(redisCommand(_redCtx, "AUTH %s",
			_password.c_str()));

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			ret = true;
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}
		return ret;
	}

	/**
	 *@brief 初始化链接信息并且链接。
	 *
	 * 初始化链接需要的信息。如果需要　password 非空那么就自动　auth 认证。
	 *
	 *@param [in]  host .  redis-server 的 ip。
	 *@param [in] port .redis-server 的 port 。
	 *@param [in] password . redis-server 认证密码，此参数可以不填，不填旧不会发起认证。
	 *@param [in] timeout . 链接的超时时间。
	 *@return true 成功，　false 失败。
	 */
	bool connect( const std::string &host , const uint16_t port , const std::string& password =
	                "" , const uint32_t timeout = 0 )
	{
		// Init attribute.
		init(host, port, password, timeout);

		return (connect());
	}

	/**
	 * @brief 检查与服务器网络
	 *
	 * @return 网络链接正常返回　true, 不通返回　false.
	 */
	bool ping( )
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		redisReply* reply = static_cast<redisReply*>(redisCommand(_redCtx, "PING"));
		bool ret = false;

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			ret = true;
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	/**
	 *@brief 重新链接　redis 服务器。
	 *
	 *@return 重连成功　true, 重连失败　false.
	 */
	bool reconnect( )
	{
		return (connect());
	}

	/**
	 * @brief 检查是否链接过　redis 服务器。
	 *
	 * @return 已经链接　true,没有链接过　false.
	 */
	inline bool isConneced( )
	{
		return _connected;
	}

	/**
	 * @brief 获取错误的原因
	 *
	 * ＠return 返回发生错误的原因.
	 */
	const std::string getErrorStr() const
	{
		return _errStr;
	}

	/**
	 * @brief 直接发送指令参数给　redis 服务器。
	 *
	 * ＠param [in] format 格式化子复制。
	 * ＠param [in] ... 指令及数据字符串。
	 * @return NULL ,redis 执行失败，或者断开连接。成功返回　redisReply 指针。
	 */
	redisReply* redisCmd( const char *format , ... )
	{
		va_list ap;
		va_start(ap, format);
		redisReply* reply = static_cast<redisReply*>(redisvCommand(_redCtx, format, ap));
		va_end(ap);
		return reply;
	}

	///////////////////////////////// list 的方法 /////////////////////////////////////
	/**
	 * @brief 从list左边插入一个元素
	 * @param [in] retval 插入成功后list长度
	 * @return false插入失败，true插入成功
	 */
	bool lpush( const std::string& key , const std::string& value , uint64_t& retval )
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		retval = 0;
		bool ret = false;

		redisReply* reply = redisCmd("LPUSH %s %s", key.c_str(), value.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			retval = reply->integer;
			ret = true;
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}
		return ret;

	}

	/**
	 * @brief 从list左边弹出一个元素
	 * @param [in] value 弹出的元素值
	 * @return false弹出失败，true弹出成功
	 */
	bool lpop( const std::string& key , std::string& value )
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply* reply = redisCmd("LPOP %s", key.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			// 失败
			if (NULL == reply->str)
			{
				_errStr = _errDes[ERR_NO_KEY];
				value = "";
				ret = false;
			}
			else
			{
				value = reply->str;
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	/**
	 * @brief 获取list指定区间内的元素
	 * @param [in] start 区间开始下标，stop 区间结束下标, valueList区间list
	 */
	bool lrange( const std::string &key , uint32_t start , int32_t end, ValueList& valueList )
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply* reply = redisCmd("LRANGE %s %d %d", key.c_str(), start, end);

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			if (REDIS_REPLY_ARRAY == reply->type && 0 == reply->elements) //<  key是list类型但 start > end
			{
				_errStr = std::string(_errDes[ERR_INDEX]) + " or "
					+ _errDes[ERR_NO_KEY];
				ret = false;

			}
			else
			{
				_getArryToList(reply, valueList);
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	/**
	 * @brief 从list右边插入一个元素
	 * @param [in] retval 插入成功后list长度
	 * @return false插入失败，true插入成功
	 */
	bool rpush(const std::string& key, const std::string& value, uint64_t& retval)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		retval = 0;
		bool ret = false;

		redisReply* reply = redisCmd("RPUSH %s %s", key.c_str(), value.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			retval = reply->integer;
			ret = true;
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	/**
	 * @brief 从list右边弹出一个元素
	 * @param [in] value 弹出的元素值
	 * @return false弹出失败，true弹出成功
	 */
	bool rpop( const std::string& key , std::string& value )
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply* reply = redisCmd("RPOP %s", key.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			// 失败
			if (NULL == reply->str)
			{
				_errStr = _errDes[ERR_NO_KEY];
				value = "";
				ret = false;
			}
			else
			{
				value = reply->str;
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	/**
	 * @brief LINSERT
	 * list의 특정 위치에 데이터 넣기
	 */
	bool linsert(const std::string& key, INSERT_POS position, const std::string& pivot,
		const std::string& value, int64_t& retval)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		std::string pos;

		if (BEFORE == position)
		{
			pos = "BEFORE";
		}
		else if (AFTER == position)
		{
			pos = "AFTER";
		}

		bool ret = false;
		redisReply* reply = redisCmd("LINSERT %s %s %s %s", key.c_str(), pos.c_str(),
			pivot.c_str(), value.c_str());

		if (_getError(reply))	//< 不是list 类型
		{
			ret = false;
		}
		else
		{
			if (REDIS_REPLY_INTEGER == reply->type)
			{
				if (reply->integer == -1)
				{
					_errStr = _errDes[ERR_NO_PIVOT];
					ret = false;
				}
				else if (reply->integer == 0)
				{
					_errStr = _errDes[ERR_NO_KEY];
					ret = false;
				}
				else
				{
					retval = reply->integer;
					ret = true;
				}
			}
			else
			{
				ret = false;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	/**
	 * @brief LINDEX
	 * lindex key index
	 * index의 위치에 있는 데이터를 조회한다. 0 에서 시작한다
	 */
	bool lindex( const std::string& key , int32_t index , std::string& value )
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply* reply = redisCmd("LINDEX %s %d", key.c_str(), index);

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			// 失败
			if (REDIS_REPLY_NIL == reply->type)
			{
				_errStr = std::string(_errDes[ERR_NO_KEY]) + " or " +
					_errDes[ERR_INDEX];
				value = "";
				ret = false;
			}
			else
			{
				value = reply->str;
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	/**
	 * @brief LLEN
	 * list에서 value의 개수를 조회 https://kwoncharlie.blog.me/220397447626
	 */
	bool llen( const std::string& key , uint64_t& retval )
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply* reply = redisCmd("LLEN %s", key.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			if (REDIS_REPLY_INTEGER == reply->type && (0 == reply->integer))
			{
				_errStr = _errDes[ERR_NO_KEY];
				ret = false;
			}
			else
			{
				retval = reply->integer;
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	//////////////////////////////   hash 的方法 //////////////////////////////////////

	/**
	 * @brief从哈希表中取出以key和field所对应的value值
	 * @param [in] key 是键名，相当于表名
	 * @param [in] filed 是字段名
	 * @param [out] value 是获取的值
	 * @return true 成功获取，false获取失败
	 * @warning 获取失败 value为""(string初始化默认值)
	 */
	bool hget(const std::string& key, const std::string& filed, std::string& value)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply* reply = redisCmd("HGET %s %s", key.c_str(), filed.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			if (REDIS_REPLY_NIL == reply->type)
			{
				_errStr = std::string(_errDes[ERR_NO_KEY]) + " or " +
					_errDes[ERR_NO_FIELD];
				ret = false;
			}
			else
			{
				value = reply->str;
				ret = true;
			}
		}
		if (NULL != reply)
		{
			freeReplyObject(reply);
		}
		else
		{

		}

		return ret;
	}
	

	/**
	 * @brief设置哈希表中以key和field所对应的value值
	 * @param [in] key 是键名，相当于表名
	 * @param [in] filed 是字段名
	 * @param [in] value 是以上两参数对应的值
	 * @param [out] retval 0:field已存在且覆盖了value ； 1： field不存在，新建field且成功设置了value
	 * @return true 成功获取，false获取失败
	 * @warning 设置失败 retval为0，成功为1
	 */
	bool hset( const std::string& key, const std::string& filed ,const std::string& value, uint32_t& retval )
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply* reply = redisCmd("HSET %s %s %s", key.c_str(), filed.c_str(), value.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			retval = (uint32_t)reply->integer;
			ret = true;
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	/**
	 * @brief删除哈希表中key所对应的field这一项内容
	 * @param [in] key 是键名，相当于表名
	 * @param [in] filed 是字段名
	 * @param [out] retval：删除的field个数
	 * @return true 成功获取，false获取失败
	 *@warning 删除失败 retval为0，成功为1
	 */
	bool hdel(const std::string& key, const std::string& filed, uint32_t& retval)
	{
		{
			if (!_connected || !_redCtx)
			{
				_errStr = _errDes[ERR_NO_CONNECT];
				return false;
			}

			bool ret = false;
			redisReply* reply = redisCmd("HDEL %s %s", key.c_str(), filed.c_str(), retval);

			if (_getError(reply))
			{
				ret = false;
			}
			else
			{
				if (REDIS_REPLY_INTEGER == reply->type && 0 == reply->integer)
				{
					_errStr = std::string(_errDes[ERR_NO_KEY]) + " or " + _errDes[ERR_NO_FIELD];
				}
				//std::cout<<"type = "<<reply->type<<"   integer = "<< reply->integer<<std::endl;
				//std::cout<<"str = " << reply->str<<std::endl;
				else
				{
					retval = (uint32_t)reply->integer;
					ret = true;
				}
			}

			if (NULL != reply)
			{
				freeReplyObject(reply);
			}

			return ret;
		}
	}

	/**
	 * @brief取得哈希表中key所对应的所有内容
	 * @param [in] key 是键名，相当于表名
	 * @param [out] valueMap 获取的值（map<string,string>类型）
	 * @return true 成功获取，false获取失败
	 * @warning 获取失败 valueMap为空
	 */
	bool hgetall(const std::string& key, ValueMap& valueMap)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply* reply = redisCmd("HGETALL %s", key.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{

			if (REDIS_REPLY_ARRAY == reply->type && 0 == reply->elements)
			{
				_errStr = _errDes[ERR_NO_KEY];

			}
			else
			{
				_getArryToMap(reply, valueMap);
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

protected:
	/**
	 *@brief  从 reply->type 为REDIS_REPLY_ARRY 类型的元素获取数据填充到　valueList 列表.
	 *
	 *@param [in] reply  . the data return from redis-server.
	 *@param [out] valueList .从　reply 提取的多条数据.
	 *@return 成功返回 true, 失败返回　false.
	 */
	bool _getArryToList( redisReply* reply , ValueList& valueList )
	{
		if (NULL == reply)
		{
			return false;
		}

		std::size_t num = reply->elements;

		for (std::size_t i = 0; i < num; i++)
		{
			valueList.push_back(reply->element[i]->str);
		}

		//	ValueList::iterator it = valueList.begin();
		//
		//	for( ; it != valueList.end(); it++ )
		//	{
		//		std::cout << "valueList: "<< *it << std::endl;
		//	}
		return true;
	}

	/**
	 *@brief  从 reply->type 为REDIS_REPLY_ARRY 类型的元素获取数据填充到valueMap 列表.
	 *
	 *hgetall 返回数据奇数为字段名,偶数为字段数据。以　字段名=字段数据的方式存在　valueMap 里。
	 *
	 *@param [in] reply  . the data return from redis-server.
	 *@param [out] valueMap .从 reply 提取的much of pair 数据存储到　valueMap.
	 *@return 成功返回 true, 失败返回　false.
	 */
	bool _getArryToMap( redisReply* reply , ValueMap& valueMap )
	{
		if (NULL == reply)
		{
			return false;
		}

		std::size_t num = reply->elements;

		for (std::size_t i = 0; i < num; i += 2)
		{
			valueMap.insert(
				std::pair<std::string, std::string>(reply->element[i]->str,
					reply->element[i + 1]->str));
		}

		return true;
	}

	bool _getError( const redisContext* redCtx )
	{
		_errStr = _errDes[ERR_NO_ERROR];
		if (redCtx == NULL)
		{
			_errStr = _errDes[ERR_NULL];
			return true;
		}
		if (redCtx->err != 0)
		{
			_errStr = redCtx->errstr;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool _getError( const redisReply* reply )
	{
		_errStr = _errDes[ERR_NO_ERROR];
		if (reply == NULL)
		{
			_errStr = _errDes[ERR_NULL];
			return false;
		}

		// have error
		if (reply->type == REDIS_REPLY_ERROR)
		{
			_errStr = reply->str;
			return true;
		}
		else
		{
			return false;
		}
	}

	inline redisContext* _getCtx( ) const
	{
		return _redCtx;
	}

private:
	DISALLOW_COPY_AND_ASSIGN( CRedisConn );

	redisContext *_redCtx;		///< redis connector context

	std::string _host;         		///< redis host
	uint16_t _port;         		///< redis sever port
	std::string _password;         	///< redis server password
	uint32_t _timeout;      		///< connect timeout second
	bool _connected;			///< if connected

	///< error number
	enum E_ERROR_NO
	{
		ERR_NO_ERROR = 0,
		ERR_NULL,
		ERR_NO_CONNECT,
		ERR_NO_PIVOT,
		ERR_NO_KEY,
		ERR_NO_FIELD,
		ERR_INDEX,
		ERR_BOTTOM
	};
	std::string _errStr;		///< Describe the reason for error..

	static const char* _errDes[ERR_BOTTOM];	///< describe error
};

const char* CRedisConn::_errDes[ERR_BOTTOM] =
{
		"No error.",
		"NULL pointer ",
		"No connection to the redis server.",
				"Inser Error,pivot not found.",
				"key not found",
		"hash field not found",
		"error index"
};

} /* namespace RedisCpp */


