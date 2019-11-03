#pragma once

#include "RedisTaskDefine.h"
#include "ErrorCode.h"

#include "../thirdparty/CRedisConn.h"
#include <vector>
#include <deque>
#include <thread>
#include <mutex>

class RedisManager
{
public:
	RedisManager() = default;
	~RedisManager() = default;

	bool Run(std::string ip_, UINT16 port_, const UINT32 threadCount_)
	{
		if (Connect(ip_, port_) == false)
		{
			printf("Redis 접속 실패\n");
			return false;
		}

		mIsTaskRun = true;

		for (UINT32 i = 0; i < threadCount_; i++)
		{
			mTaskThreads.emplace_back([this]() { TaskProcessThread(); });
		}

		printf("Redis 동작 중...\n");
		return true;
	}

	void End()
	{
		mIsTaskRun = false;

		for (auto& thread : mTaskThreads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
	}

	void PushTask(RedisTask task_)
	{
		std::lock_guard<std::mutex> guard(mReqLock);
		mRequestTask.push_back(task_);
	}

	RedisTask TakeResponseTask()
	{
		std::lock_guard<std::mutex> guard(mResLock);

		if (mResponseTask.empty())
		{
			return RedisTask();
		}

		auto task = mResponseTask.front();
		mResponseTask.pop_front();

		return task;
	}


private:
	bool Connect(std::string ip_, UINT16 port_)
	{
		if (mConn.connect(ip_, port_) == false)
		{
			std::cout << "connect error " << mConn.getErrorStr() << std::endl;
			return false;
		}
		else
		{
			std::cout << "connect success !!!" << std::endl;
		}

		return true;
	}

	void TaskProcessThread()
	{
		printf("Redis 스레드 시작...\n");

		while (mIsTaskRun)
		{
			bool isIdle = true;

			if (auto task = TakeRequestTask(); task.TaskID != RedisTaskID::INVALID)
			{
				isIdle = false;

				if (task.TaskID == RedisTaskID::REQUEST_LOGIN)
				{
					auto pRequest = (RedisLoginReq*)task.pData;
					
					RedisLoginRes bodyData;
					bodyData.Result = (UINT16)ERROR_CODE::LOGIN_USER_INVALID_PW;

					std::string value;
					if (mConn.get(pRequest->UserID, value))
					{
						bodyData.Result = (UINT16)ERROR_CODE::NONE;

						if (value.compare(pRequest->UserPW) == 0)
						{
							bodyData.Result = (UINT16)ERROR_CODE::NONE;
						}
					}
					
					RedisTask resTask;
					resTask.UserIndex = task.UserIndex;
					resTask.TaskID = RedisTaskID::RESPONSE_LOGIN;
					resTask.DataSize = sizeof(RedisLoginRes);
					resTask.pData = new char[resTask.DataSize];
					CopyMemory(resTask.pData, (char*)&bodyData, resTask.DataSize);

					PushResponse(resTask);
				}
				
				task.Release();
			}

			
			if (isIdle)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}

		printf("Redis 스레드 종료\n");
	}

	RedisTask TakeRequestTask()
	{
		std::lock_guard<std::mutex> guard(mReqLock);

		if (mRequestTask.empty())
		{
			return RedisTask();
		}

		auto task = mRequestTask.front();
		mRequestTask.pop_front();

		return task;
	}

	void PushResponse(RedisTask task_)
	{
		std::lock_guard<std::mutex> guard(mResLock);
		mResponseTask.push_back(task_);
	}



	RedisCpp::CRedisConn mConn;

	bool		mIsTaskRun = false;
	std::vector<std::thread> mTaskThreads;

	std::mutex mReqLock;
	std::deque<RedisTask> mRequestTask;

	std::mutex mResLock;
	std::deque<RedisTask> mResponseTask;
};