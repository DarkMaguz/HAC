/*
 * main.cpp
 *
 *  Created on: 16/07/2009
 *      Author: Peter S. Balling
 */

#include "HAC.h"

#include <iostream>
#include <cmath>
#include <map>
#include <algorithm>
#include <csignal>
#include <cerrno>
using namespace std;

const char useage[] =
{
	"\n\tUsage: { hac } [ OPTION ]\n\n" \
	"\tOptions\t\t"			"Long options\t\t"				"Descriptions\n" \
	"\t -r USER EMAIL\t"	" --register USER EMAIL\t"	" Register a new user.\n" \
	"\t -a\t\t"				" --activate\t\t"					" Activate user.\n" \
	"\t -c\t\t"				" --configure\t\t"					" Run configuration.\n" \
	"\t -u USER\t"			" --user USER\t\t"				" Change user.\n" \
	"\t -b\t\t"				" --benchmark\t\t"				" Run benchmark test.\n" \
	"\t -d PATH\t"			" --debug PATH\t\t"				" Debug work unit.\n" \
	"\t -h\t\t"				" --help\t\t\t"						" This message.\n"
};

enum args
{
	cmd_r = 1,
	cmd_a,
	cmd_c,
	cmd_u,
	cmd_b,
	cmd_d,
	cmd_h
};

int main(int argc, char **argv)
{
	map<string, args> arg_map;
	arg_map["-r"] = cmd_r;
	arg_map["--register"] = cmd_r;
	arg_map["-a"] = cmd_a;
	arg_map["--activate"] = cmd_a;
	arg_map["-c"] = cmd_c;
	arg_map["--configure"] = cmd_c;
	arg_map["-u"] = cmd_u;
	arg_map["--user"] = cmd_u;
	arg_map["-b"] = cmd_b;
	arg_map["--benchmark"] = cmd_b;
	arg_map["-d"] = cmd_d;
	arg_map["--debug"] = cmd_d;
	arg_map["-h"] = cmd_h;
	arg_map["--help"] = cmd_h;
	
	if (argc > 4)
	{
		cout << useage << endl;
		return 0;
	}
	
	if (argc == 2)
	{
		if (arg_map[argv[1]] == cmd_h)
		{
			cout << useage << endl;
			return 0;
		}
	}
	
	cout << "Loading..." << endl;
	
	HAC client;
	if (!client.cfg_ok)
		return 0;
	
	if (argc != 1)
	{
		switch (arg_map[argv[1]])
		{
			case cmd_r:
			{
				if (argc < 3)
				{
					cout << useage << endl;
					return 0;
				}
				
				client.user = argv[2];
				
				client.Register(argv[3]);
				
				break;
			}
			case cmd_a:
			{
				client.Activate();
				
				break;
			}
			case cmd_c:
			{
				client.Configure();
				
				break;
			}
			case cmd_u:
			{
				client.user = argv[2];
				client.SaveConfig();
				
				break;
			}
			case cmd_b:
			{
				cout << "Score: " << client.BenchMark() << endl;
				
				break;
			}
			case cmd_d:
			{
				client.Debug(argv[2]);
				
				break;
			}
			case cmd_h:
			{
				cout << useage << endl;
				return 0;
				
				break;
			}
			default:
				break;
		}
	}
	else
	{
		client.Start();
	}
	
	return 0;
}
