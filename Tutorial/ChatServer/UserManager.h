#pragma once
#include <unordered_map>

#include "ErrorCode.h"
#include "User.h"

class UserManager
{
public:
	UserManager() = default;
	~UserManager() = default;

	void Init(const INT32 maxUserCount)
	{
		m_MaxUserCnt = maxUserCount;
		UserObjPool = std::vector<User*>(m_MaxUserCnt);

		for (auto i = 0; i < m_MaxUserCnt; i++)
		{
			UserObjPool[i] = new User();
			UserObjPool[i]->Init(i);
		}
	}

	INT32 GetCurrentUserCnt() { return m_CurrentUserCnt; }

	INT32 GetMaxUserCnt() { return m_MaxUserCnt; }
		
	void IncreaseUserCnt() { m_CurrentUserCnt++; }
		
	void DecreaseUserCnt() 
	{
		if (m_CurrentUserCnt > 0) 
		{
			m_CurrentUserCnt--;
		}
	}

	ERROR_CODE AddUser(char* userID, int conn_idx)
	{
		//TODO 최흥배 유저 중복 조사하기

		auto user_idx = conn_idx;

		UserObjPool[user_idx]->SetLogin(userID);
		UserDictionary.insert(std::pair< char*, int>(userID, conn_idx));

		return ERROR_CODE::NONE;
	}
		
	INT32 FindUserIndexByID(char* userID)
	{
		if (auto res = UserDictionary.find(userID); res != UserDictionary.end())
		{
			return (*res).second;
		}
			
		return -1;
	}
		
	void DeleteUserInfo(User* deleteUser)
	{
		UserDictionary.erase(deleteUser->GetUserId());
		deleteUser->Clear();
	}

	User* GetUserByConnIdx(INT32 conn_idx)
	{
		return UserObjPool[conn_idx];
	}


private:
	INT32 m_MaxUserCnt = 0;
	INT32 m_CurrentUserCnt = 0;

	std::vector<User*> UserObjPool; //vector로
	std::unordered_map<std::string, int> UserDictionary;
};
