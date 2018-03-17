/*
 * Config.cpp
 *
 *  Created on: 18/07/2009
 *      Author: Peter S. Balling
 */

#include "Config.h"
#include "Utils.h"

bool is_space(char x)
{
	return isspace(x);
}

Config::Config()
{
	cpu_max = sysconf( _SC_NPROCESSORS_ONLN);
	
	cfg_map["user"] = cfg_user;
	cfg_map["id"] = cfg_id;
	cfg_map["cores"] = cfg_cores;
	cfg_map["ram"] = cfg_ram;
	cfg_map["benchmark"] = cfg_benchmark;
	
	cfg_ok = false;
	
	user_id = 0;
	core_count = 0;
	ram_percentage = 0;
	benchmark = 0;
}

Config::~Config()
{
}

void Config::Configure(void)
{
	// Configure core count
	uint64_t choice = 0;
	string input;
	uint64_t core_found;
	
	cout << "Detecting number of cpu's/core's...\t";
	cout.flush();
	
	core_found = sysconf( _SC_NPROCESSORS_ONLN);
	
	if (core_count < 1 || core_count > core_found)
		core_count = core_found;
	
	cout << core_found << " found." << endl;
	
	if (core_found > 1)
	{
		while (choice < 1 || choice > core_found)
		{
			cout << "How many do you wish to use[" << core_count << "]: ";
			
			getline(cin, input);
			
			if (input.empty())
				choice = core_count;
			else
				choice = atoi(input.c_str());
		}
		core_count = choice;
	}
	else
	{
		core_count = core_found;
	}
	
	// Configure RAM
	choice = 0;
	input.clear();
	
	if (ram_percentage < RAM_PERCENTAGE_MIN || ram_percentage > RAM_PERCENTAGE_MAX)
		ram_percentage = RAM_PERCENTAGE_MAX;
	
	while (choice < RAM_PERCENTAGE_MIN || choice > RAM_PERCENTAGE_MAX)
	{
		cout << "How many percentage of available RAM do you wish to use[" << ram_percentage << "]: ";
		
		getline(cin, input);
		
		if (input.empty())
			choice = ram_percentage;
		else
			choice = atoi(input.c_str());
	}
	ram_percentage = choice;
	
	// Configure user
	bool status_good = false;
	input.clear();
	
	while (!status_good)
	{
		cout << "Username";
		
		if (user.empty())
			cout << ": ";
		else
			cout << "[" << user << "]: ";
		
		getline(cin, input);
		
		if (input.empty())
		{
			if (!user.empty())
				status_good = true;
		}
		else
		{
			const static string goodchars = "-_.";
			uint64_t userlen = input.length();
			
			if (userlen < 6)
			{
				cout << "Invalid user: To short, it must be at least 6." << endl;
			}
			else
			{
				status_good = true;
				for (uint64_t i = 0; i < userlen; i++)
					if (!isalnum(input[i]))
					{
						if (i == 0 || i == userlen - 1)
						{
							cout << "Invalid user: First and last sign must be alphanumeric." << endl;
							status_good = false;
							break;
						}
						else if (goodchars.find(input[i]) == string::npos)
						{
							cout << "Invalid user: Illegal sign \"" << input[i] << "\"." << endl;
							status_good = false;
							break;
						}
					}
			}
			
			if (status_good)
				user = input;
		}
	}
	
	cfg_ok = SaveConfig();
}

bool Config::SaveConfig(void)
{
	
	cout << "Updating \"" << CONFIG_PATH << "\"...\t";
	cout.flush();
	if (file_exists( CONFIG_PATH))
		rename( CONFIG_PATH, CONFIG_PATH_BAK);
	
	fstream fs( CONFIG_PATH, ios::out | ios::trunc);
	if (fs.is_open())
	{
		fs << "user\t= " << user << endl;
		fs << "id\t= " << user_id << endl;
		fs << "cores\t= " << core_count << endl;
		fs << "ram\t= " << ram_percentage << endl;
		fs << "benchmark\t= " << benchmark << endl;
		
		fs.close();
		
		cout << "[SUCCESS]" << endl;
		
		if (file_exists( CONFIG_PATH_BAK))
			remove( CONFIG_PATH_BAK);
	}
	else
	{
		if (file_exists( CONFIG_PATH_BAK))
		{
			if (file_exists( CONFIG_PATH))
				remove( CONFIG_PATH);
			rename( CONFIG_PATH_BAK, CONFIG_PATH);
		}
		
		cerr << "[FAILED]" << endl;
		cerr << "Error writing \"" << CONFIG_PATH << "\"." << endl;
		
		return false;
	}
	
	return true;
}

void Config::LoadConfig(void)
{
	cfg_ok = false;
	
	cout << "File: \"" << CONFIG_PATH << "\"\t";
	cout.flush();
	
	if (!file_exists( CONFIG_PATH))
	{
		cout << "Not found." << endl;
		cout << "Running configuration:" << endl;
		Configure();
	}
	else
	{
		fstream fs( CONFIG_PATH);
		if (fs.is_open())
		{
			string cfg_line;
			uint64_t found;
			
			while (!fs.eof())
			{
				cfg_line.clear();
				
				getline(fs, cfg_line);
				
				cfg_line.erase(remove_if(cfg_line.begin(), cfg_line.end(), is_space), cfg_line.end());
				
				found = cfg_line.find('#');
				if (found != string::npos)
					cfg_line.erase(found);
				
				if (!cfg_line.empty())
				{
					found = cfg_line.find('=');
					
					switch (cfg_map[cfg_line.substr(0, found)])
					{
						case cfg_user:
						{
							user = cfg_line.substr(found + 1);
							break;
						}
						case cfg_id:
						{
							user_id = atoll(cfg_line.substr(found + 1).c_str());
							break;
						}
						case cfg_cores:
						{
							core_count = atoll(cfg_line.substr(found + 1).c_str());
							break;
						}
						case cfg_ram:
						{
							ram_percentage = atoll(cfg_line.substr(found + 1).c_str());
							break;
						}
						case cfg_benchmark:
						{
							benchmark = atoll(cfg_line.substr(found + 1).c_str());
							break;
						}
						default:
							break;
					}
				}
			}
			fs.close();
			
			if (core_count > (uint64_t)sysconf( _SC_NPROCESSORS_ONLN))
			{
				cerr << "[FAILED]" << endl;
				cerr << "Error getting correct number of cpu's/core's from \"" << CONFIG_PATH << "\"." << endl;
				cerr << CONFIG_PATH << " asked for " << core_count << " cpu's/core's, while system only has " << (uint64_t)sysconf( _SC_NPROCESSORS_ONLN) << endl;
				cout << "Running configuration:" << endl;
				Configure();
			}
			else if (core_count == 0)
			{
				cerr << "[FAILED]" << endl;
				cerr << "Error getting correct number of cpu's/core's from \"" << CONFIG_PATH << "\"." << endl;
				cerr << CONFIG_PATH << " asked for 0 it must be at least 1" << endl;
				cout << "Running configuration:" << endl;
				Configure();
			}
			else if (ram_percentage < RAM_PERCENTAGE_MIN || ram_percentage > RAM_PERCENTAGE_MAX)
			{
				cerr << "[FAILED]" << endl;
				cerr << "Error getting correct ram usage from \"" << CONFIG_PATH << "\"." << endl;
				cerr << CONFIG_PATH << " asked for " << ram_percentage << "% of available ram," << endl;
				cerr << "it has to be whit in the range from " << RAM_PERCENTAGE_MIN << " to " << RAM_PERCENTAGE_MAX << endl;
				cout << "Running configuration:" << endl;
				Configure();
			}
			else if (user.empty())
			{
				cerr << "[FAILED]" << endl;
				cerr << "Error getting user name from \"" << CONFIG_PATH << "\"." << endl;
				cout << "Running configuration:" << endl;
				Configure();
			}
			else
			{
				cout << "[SUCCESS]" << endl;
				cfg_ok = true;
			}
		}
		else
		{
			cerr << "[FAILED]" << endl;
			cerr << "Error opening \"" << CONFIG_PATH << "\"." << endl;
		}
	}
}
