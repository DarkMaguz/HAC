/*
 * Config.h
 *
 *  Created on: 18/07/2009
 *      Author: Peter S. Balling
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "NetStream.h"
#include "Utils.h"

#include <iostream>
#include <cstring>
#include <string>
#include <map>
#include <cstdlib>
#include <algorithm>
using namespace std;

#include <sys/sysinfo.h>

#define RAM_PERCENTAGE_MAX 95
#define RAM_PERCENTAGE_MIN 10
#define CONFIG_PATH "config.cfg"
#define CONFIG_PATH_BAK "config.cfg.bak"

bool is_space(char x);

enum cfg_args
{
	cfg_user = 0,
	cfg_id,
	cfg_cores,
	cfg_ram,
	cfg_benchmark,
	cfg_0
};

class Config
{
	public:
		Config();
		virtual ~Config();
		
		void Configure(void);
		bool SaveConfig(void);
		void LoadConfig(void);
		
		string user;
		bool cfg_ok;
		
	protected:
		uint64_t user_id;
		uint64_t core_count;
		uint64_t ram_percentage;
		uint64_t benchmark;
		uint64_t cpu_max;
		
	private:
		map<string, cfg_args> cfg_map;
};

#endif /* CONFIG_H_ */
