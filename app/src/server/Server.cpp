/*
 * Server.cpp
 *
 *  Created on: 25. feb. 2018
 *      Author: magnus
 */

#include "Server.h"

Server::Server() :
	log( "log" ),
	mutex_user( (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER ),
	mutex_workunit( (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER ),
	mutex_collision( (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER ),
	mutex_wubank( (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER )
{
	
	log.AddFile( "main.log" ); // 0
	
	log.AddFile( "new.log" ); // 1
	log.AddFile( "activate.log" ); // 2
	log.AddFile( "auth.log" ); // 3
	
	log.AddFile( "get.log" ); // 4
	log.AddFile( "send.log" ); // 5
	log.AddFile( "free.log" ); // 6
	log.AddFile( "collision.log" ); // 7
	
	log.AddFile( "mysql.log" ); // 8
	
}

Server::~Server()
{
	pthread_mutex_destroy( &mutex_user );
	pthread_mutex_destroy( &mutex_workunit );
	pthread_mutex_destroy( &mutex_collision );
	pthread_mutex_destroy( &mutex_wubank );
}
