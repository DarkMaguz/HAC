/*
 * Server.h
 *
 *  Created on: 25. feb. 2018
 *      Author: magnus
 */

#ifndef APP_SRC_SERVER_SERVER_H_
#define APP_SRC_SERVER_SERVER_H_

#include "Log.h"

#include <pthread.h>

class Server
{
	public:
		Server();
		virtual ~Server();
		Log log;
		
		pthread_mutex_t mutex_user;
		pthread_mutex_t mutex_workunit;
		pthread_mutex_t mutex_collision;
		pthread_mutex_t mutex_wubank;
};

#endif /* APP_SRC_SERVER_SERVER_H_ */
