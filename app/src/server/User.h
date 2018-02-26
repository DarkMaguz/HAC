/*
 * User.h
 *
 *  Created on: 26. feb. 2018
 *      Author: magnus
 */

#ifndef APP_SRC_SERVER_USER_H_
#define APP_SRC_SERVER_USER_H_

#include "Socket.h"
#include "Server.h"
#include "NetStream.h"

#include <string>
using namespace std;

#define DB_HOST_NAME "hacdb"
#define DB_NAME "hac01"
#define DB_USER "hac"
#define DB_PASS "cah"

class User
{
	public:
		User(Socket sock, Server *server);
		virtual ~User();
		
		bool Activate( void );
		bool Get( void );
		
		NetStream ns;
		uint64_t sessionStart;
		
		Server *server;
		
		uint32_t id;
		string name;
		string email;
		uint64_t score;
		string validation;
		uint32_t wu_completed;
		uint32_t wu_inprogress;
		uint32_t wu_total;
};

#endif /* APP_SRC_SERVER_USER_H_ */
