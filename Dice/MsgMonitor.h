/*
 * Copyright (C) 2019 String.Empty
 * 消息频率监听
 */
#pragma once
#include <map>
#include <ctime>
#include "DiceConsole.h"
#include "ManagerSystem.h"
#include "DiceCloud.h"
class FrqMonitor {
public:
	static const long long earlyTime = 30;
	static const long long earlierTime = 60;
	static const long long earliestTime = 300;
	//频率记录
	static std::map<long long, int>mFrequence;
	static std::map<long long, int>mWarnLevel;
	static int getFrqTotal();
	long long fromQQ = 0;
	time_t fromTime = 0;
	FrqMonitor(long long QQ,time_t TT,chatType CT): fromQQ(QQ),fromTime(TT){
		if (mFrequence.count(fromQQ)) {
			mFrequence[fromQQ] += 10;
			if (!console["ListenSpam"] || trustedQQ(fromQQ) > 1 )return;
			if (mFrequence[fromQQ] > 60 && mWarnLevel[fromQQ] < 60) {
				mWarnLevel[fromQQ] = mFrequence[fromQQ];
				const std::string strMsg = "提醒：\n" + (CT.second ? printChat(CT) : "私聊窗口") + "监测到" + printQQ(fromQQ) + "指令频度达到" + std::to_string(mFrequence[fromQQ] / 10);
				AddMsgToQueue(GlobalMsg["strSpamFirstWarning"], CT.first, CT.second);
				console.log(strMsg, 1, printSTNow());
			}
			else if (mFrequence[fromQQ] > 120 && mWarnLevel[fromQQ] < 120) {
				mWarnLevel[fromQQ] = mFrequence[fromQQ]; 
				const std::string strMsg = "警告：\n" + (CT.second ? printChat(CT) : "私聊窗口") + printQQ(fromQQ) + "指令频度达到" + std::to_string(mFrequence[fromQQ] / 10);
				AddMsgToQueue(GlobalMsg["strSpamFinalWarning"], CT.first, CT.second);
				console.log(strMsg, 3, printSTNow());
			}
			else if (mFrequence[fromQQ] > 200 && mWarnLevel[fromQQ] < 200) {
				mWarnLevel[fromQQ] = mFrequence[fromQQ];
				std::string strNow = printSTNow();
				std::string strNote = (CT.second ? printChat(CT) : "私聊窗口") + "监测到" + printQQ(fromQQ) + "对" + printQQ(DiceMaid) + "30s发送指令频度达" + std::to_string(mFrequence[fromQQ] / 10);
				if ( mDiceList.count(fromQQ)) {
					console.log(strNote, 9, strNow);
				}
				else {
					BlackMark mark(fromQQ);
					mark.llMap = { {"fromQQ",fromQQ},{"DiceMaid",DiceMaid},{"masterQQ", console.master()} };
					mark.strMap = { {"type","spam"},{"time",strNow} };
					mark.set("note", strNow + strNote);
					Cloud::upWarning(mark.getData());
					console.log(mark.getWarning(), 33, "");
					addBlackQQ(mark);
				}
			}
		}
		else {
			mFrequence[fromQQ] = 10;
			mWarnLevel[fromQQ] = 0;
		}
	}
	~FrqMonitor() {
		if (mWarnLevel[fromQQ])mWarnLevel[fromQQ] -= 10;
	}
	bool isEarliest() {
		if (time(NULL) - fromTime > earliestTime) {
			mFrequence[fromQQ]--;
			delete this;
			return true;
		}
		else return false;
	}
	bool isEarlier() {
		if (time(NULL) - fromTime > earlierTime) {
			mFrequence[fromQQ] -= 4;
			return true;
		}
		else return false;
	}
	bool isEarly() {
		if (time(NULL) - fromTime > earlyTime) {
			mFrequence[fromQQ] -= 4;
			return true;
		}
		else return false;
	}
};
void AddFrq(const long long QQ, time_t TT, chatType CT);
void frqHandler();