/*
 * loadhistory.cpp
 *
 *  Created on: 2016/10/16/
 *      Author: user
 */

#include "../../public/history/HistoryLoad.h"
#include "../../public/history/HistoryLoadFactory.h"
#include "../../public/program/Profile.h"

int main(int argc, char* argv[]) {
	char market_name[64];
	memset(market_name, 0, 64);
	char program[64];
	memset(program, 0, 64);
	char shm_path[256];
	memset(shm_path, 0, 256);
	char his_path[256];
	memset(his_path, 0, 256);
	int max_mink = 0;
	char avg_except_rule[64] = {0};
	char his_share_path[256] = {0};
	char vol_stat_path[256] = {0};

	Profile* pf = NULL;
	const char* v = NULL;
	int date = 0;
	bool isAll = false;

	if (argc == 2 || argc == 3) {
		strcpy(market_name, argv[1]);
		if (strcmp(market_name, "sz1") == 0)
			sprintf(program, "%sv5transfer", market_name);
		else
			sprintf(program, "%stransfer", market_name);
		pf = Profile::Instance(program);
		sprintf(program, "%s_his_shm_path", market_name);
		v = pf->getEnv(program);
		if (v == NULL) {
			ERROR("%s not found", program);
			exit(1);
		}
		strcpy(shm_path, v);

		sprintf(program, "%s_his_path", market_name);
		v = pf->getEnv(program);
		if (v == NULL) {
			ERROR("%s not found", program);
			exit(1);
		}
		strcpy(his_path, v);

		if (strcmp(market_name, "lse") != 0)
		{
			sprintf(program, "%s_max_mink", market_name);
			v = pf->getEnv(program);
			if (v == NULL)
			{
				ERROR("%s not found", program);
				exit(1);
			}
			max_mink = atoi(v);
		}

		sprintf(program, "%s_avg_except_rule", market_name);
		v = pf->getEnv(program);
		if (v != NULL) {
			strcpy(avg_except_rule, v);
		}

		sprintf(program, "%s_his_share_path", market_name);
		v = pf->getEnv(program);
		if (v != NULL) {
			strcpy(his_share_path, v);
		}

		sprintf(program, "%s_vol_stat_path", market_name);
		v = pf->getEnv(program);
		if (v != NULL) {
			strcpy(vol_stat_path, v);
		}

		if (argc == 2) {
			//current day
			time_t t;
			time(&t);
			struct tm *pt = localtime(&t);
			char chdate[9];
			snprintf(chdate, 9, "%4.4d%2.2d%2.2d", pt->tm_year + 1900, pt->tm_mon + 1,
					pt->tm_mday);
			date = atoi(chdate);
		}
		else {
			if (strcmp(argv[2], "all") == 0) {
				isAll = true;
			}
			else {
				date = atoi(argv[2]);
			}
		}
	}
	else {
		ERROR("usage: ./historyload market_name <date | all>");
		exit(1);
	}

	CShmEnv* shmEnv = new CShmEnv();
	if (shmEnv->openShmEnv(shm_path) != SUCCEED) {
		ERROR("openShmEnv(%s) error", shm_path);
		exit(1);
	}

	if (strcmp(market_name, "neeq") == 0)
		strcpy(market_name, "bj"); //match the realtime quotation

	HistoryLoadFactory fac;
	HistoryLoad* his = fac.Create(shmEnv, market_name, his_path);
	if (his == nullptr) {
		ERROR("create protocol factory failed!");
		exit(1);
	}

	//设置均价采用加权平均的代码规则
	if (strlen(avg_except_rule) > 0)
		his->setAvgExceptRule(avg_except_rule);

	//加载历史股本数据
	if (strlen(his_share_path) > 0)
	{
		if (!his->loadHisShare(his_share_path))
		{
			ERROR("load history share data failed!");
		}
		else
		{
			INFO("load history share data success.");
		}
	}

	if (isAll) {
		//加载成交量统计数据
		if (strlen(vol_stat_path) > 0)
		{
			if (!his->loadAllVolStat(vol_stat_path))
			{
				ERROR("load volume stat data failed!");
			}
			else
			{
				INFO("load volume stat data success.");
			}
			
		}

		his->Clear(); //clear all first
		//load all day klines and minute klines within limit
		if (!his->loadAll() || !his->loadAll(max_mink))
			exit(1);
	}
	else {
		//加载单日成交量统计数据
		if (strlen(vol_stat_path) > 0)
		{
			if (!his->loadVolStat(vol_stat_path, date))
			{
				ERROR("load volume stat data failed!");
			}
		}

		//load the day kline of date and minute klines of the date
		if (!his->load(date) || !his->load(date, max_mink))
			exit(1);
	}

	// 关闭共享内存
	if (shmEnv->closeShmEnv() != SUCCEED) {
		ERROR("closeShmEnv(%s) error", shm_path);
		exit(1);
	}
	delete shmEnv;

	return 0;
}


