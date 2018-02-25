/*
 * Log.h
 *
 *  Created on: 02/08/2009
 *      Author: Peter S. Balling
 */

#ifndef LOG_H_
#define LOG_H_

#include "Utils.h"
#include "Password.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdarg>
using namespace std;

#include <pthread.h>
#include <time.h>

#define DEBUG_LOG 1

struct LogFile
{
	
	LogFile( const string& path ) :
		path( path ),
		mutex_fs( (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER ),
		fs( path.c_str(), ios::out | ios::app )
	{
		
		fs << "Log session started " << str_date() << endl;
		
	}
	
	virtual ~LogFile()
	{
		
		pthread_mutex_destroy( &mutex_fs );
		
		fs << "Log session stopped " << str_date() << endl;
		
		fs.close();
		
	}
	
	const string path;
	
	pthread_mutex_t mutex_fs;
	fstream fs;
	
};

class Log
{
	public:
		Log( const string &dir );//, const string &main_log_file );
		virtual ~Log();
		
		void AddFile( const string &log_file );
		
		//void Put( const int &file_id, const uint32_t &user, const string &msg );
		
		void Put( const int &file_id, const uint32_t &user, const string &format, ... );
		
	private:
		
		//time_t rawtime;
		//struct tm *timeinfo;
		
		vector<struct LogFile *> files;
		
		const string dir;
		
};

#endif /* LOG_H_ */
