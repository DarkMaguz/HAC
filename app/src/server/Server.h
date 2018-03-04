/*
 * Server.h
 *
 *  Created on: 25. feb. 2018
 *      Author: Peter S. Balling
 */

#ifndef APP_SRC_SERVER_SERVER_H_
#define APP_SRC_SERVER_SERVER_H_

#include "Log.h"

#include <pthread.h>

#define DEFAULT_SERVER_PORT 4337
//#define SERVER_HOST_NAME "localhost"

class Server : public ServerSocket
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
