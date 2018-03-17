/*
 * HAC.h
 *
 *  Created on: 16/07/2009
 *      Author: Peter S. Balling
 */

#ifndef HAC_H_
#define HAC_H_

//#define __need_timespec

#include "WorkUnit.h"
#include "Config.h"
#include "Password.h"
#include "Hash.h"
#include "HashData.h"

#include "Socket.h"
#include "NetStream.h"
#include "Coordinates.h"
#include "Utils.h"

#include <iostream>
#include <cmath>
#include <map>
#include <algorithm>
#include <csignal>
#include <cerrno>
using namespace std;

#include <pthread.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <linux/input.h>

#define SERVER_PORT 4337
#define SERVER_HOST_NAME "localhost"
#define BENCHMARK_TIME 20
#define AUTOSAVE_TIME 300
#define WORKUNIT_PATH "workunit.dat"
#define WORKUNIT_PATH_BAK "workunit.bak"

void *Run(void *arg);
void *Build(void *arg);
void *AutoSave(void *arg);
void *SignalHandler(void *arg);

class HAC: public Config
{
		friend void *AutoSave(void *arg);
		friend void *SignalHandler(void *arg);
	public:
		HAC();
		virtual ~HAC();
		
		void Start(void);
		uint64_t BenchMark(void);
		void Debug(const string path);
		
	private:
		struct WorkUnit workUnit;
		struct sysinfo sysInfo;
		int32_t exitType;
		sigset_t signalMask;
		
		ClientSocket clientSocket;
		
		void Exit(int32_t status = 0);
		
		void ClearWU(void);
		void PrepareWU(void);
		
		void Save(void);
		void Load(void);
		
		bool Connect(void);
		bool Auth(void);
		
		void Send(void);
		void SendCollision(void);
		void Get(void);
		void Free(void);
		
	public:
		void Activate(void);
		void Register(string new_email);
};

#endif /* HAC_H_ */
