/*
 * HAC.h
 *
 *  Created on: 16/07/2009
 *      Author: Peter S. Balling
 */

#ifndef HAC_H_
#define HAC_H_

//#define __need_timespec

#include "Config.h"
#include "Password.h"
#include "Hash.h"

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

struct SubWorkUnit
{
	
	SubWorkUnit() :
		progress( 0 ),
		completed( 0 ),
		chunk_size( 0 ),
		private_id( 0 ),
		private_progress(NULL),
		private_end(NULL)
	{
		
	}
	
	pthread_mutex_t sub_mutex;
	
	uint64_t progress;
	uint64_t completed;
	
	uint64_t chunk_size;
	
	vector<struct Coordinates> collision;
	
	int32_t private_id;
	uint64_t *private_progress;
	uint64_t *private_end;
	
};

struct WorkUnit
{
	
	WorkUnit() :
		safe_to_save( false ),
		terminate( false ),
		id( 0 ),
		x_size( 0 ),
		y_size( 0 ),
		data_size( 0 ),
		data(NULL),
		data_end(NULL)
	{
		
	}
	
	pthread_cond_t save_cond;
	pthread_mutex_t save_mutex;
	bool safe_to_save;
	
	bool terminate;
	
	uint32_t id;
	
	Password x_pwd;
	uint64_t x_size;
	
	Password y_pwd;
	uint64_t y_size;
	
	uint64_t data_size;
	struct HashData *data;
	struct HashData *data_end;
	
	struct SubWorkUnit swu;
	
};

void *Run( void *arg );
void *Build( void *arg );
void *AutoSave( void *arg );
void *SignalHandler( void *arg );

class HAC : public Config
{
		friend void *AutoSave( void *arg );
		friend void *SignalHandler( void *arg );
	public:
		HAC();
		virtual ~HAC();
		
		void Start( void );
		uint64_t BenchMark( void );
		void Debug( const string path );
		
	private:
		
		struct WorkUnit wu;
		struct sysinfo info;
		int32_t exit_type;
		sigset_t signal_mask;
		
		ClientSocket cs;
		
		void Exit( int32_t status = 0 );
		
		void ClearWU( void );
		void PrepareWU( void );
		
		void Save( void );
		void Load( void );
		
		bool Connect( void );
		bool Auth( void );
		
		void Send( void );
		void SendCollision( void);
		void Get( void );
		void Free( void );
		
	public:
		
		void Activate( void );
		void Register( string new_email );
		
};

#endif /* HAC_H_ */
