#pragma once
#include <unordered_map>

#include "ErrorCode.h"
#include "User.h"

class UserManager
{
public:
	UserManager() = default;
	~UserManager() = default;

	void Init(const INT32 maxUserCount_)
	{
		mMaxUserCnt = maxUserCount_;
		mUserObjPool = std::vector<User*>(mMaxUserCnt);

		for (auto i = 0; i < mMaxUserCnt; i++)
		{
			mUserObjPool[i] = new User();
			mUserObjPool[i]->Init(i);
		}
	}

	INT32 GetCurrentUserCnt() { return mCurrentUserCnt; }

	INT32 GetMaxUserCnt() { return mMaxUserCnt; }
		
	void IncreaseUserCnt() { mCurrentUserCnt++; }
		
	void DecreaseUserCnt() 
	{
		if (mCurrentUserCnt > 0) 
		{
			mCurrentUserCnt--;
		}
	}

	ERROR_CODE AddUser(char* userID_, int clientIndex_)
	{
		//TODO 최흥배 유저 중복 조사하기

		auto user_idx = clientIndex_;

		mUserObjPool[user_idx]->SetLogin(userID_);
		mUserIDDictionary.insert(std::pair< char*, int>(userID_, clientIndex_));

		return ERROR_CODE::NONE;
	}
		
	INT32 FindUserIndexByID(char* userID_)
	{
		if (auto res = mUserIDDictionary.find(userID_); res != mUserIDDictionary.end())
		{
			return (*res).second;
		}
			
		return -1;
	}
		
	void DeleteUserInfo(User* user_)
	{
		mUserIDDictionary.erase(user_->GetUserId());
		user_->Clear();
	}

	User* GetUserByConnIdx(INT32 clientIndex_)
	{
		return mUserObjPool[clientIndex_];
	}


private:
	INT32 mMaxUserCnt = 0;
	INT32 mCurrentUserCnt = 0;

	std::vector<User*> mUserObjPool; //vector로
	std::unordered_map<std::string, int> mUserIDDictionary;
};
