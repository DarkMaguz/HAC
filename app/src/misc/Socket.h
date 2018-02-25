/*
 * Socket.h
 *
 *  Created on: 24/07/2009
 *      Author: Peter S. Balling
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <iostream>
using namespace std;

struct SocketTTL_s
{
	
	/*SocketTTL_s()
	{
		
	}
	
	SocketTTL_s( const int32_t &seconds, const int32_t &microseconds, const pthread_t &parrent_threadid ) :
		seconds( seconds ),
		microseconds( microseconds ),
		parrent_threadid( parrent_threadid )
	{
		
	}*/
	
	int32_t seconds;
	int32_t microseconds;
	
	pthread_t parrent_threadid;
	pthread_t threadid;
	pthread_attr_t pattr;
	
	void *arg;
	void *(*stop_routine) (void *);
	
	//bool is_set;
	
};

void *SocketConnect( void *arg );
void *SocketTTL( void *arg ); // Socket Time To Live

class Socket
{
		friend void *SocketTTL( void *arg );
	public:
		Socket();
		//Socket( const Socket &x );
		Socket( const int32_t sockfd, const int32_t port );
		virtual ~Socket();
		
		void SetTTL( const int32_t &seconds, const int32_t &microseconds, void *(*stop_routine) (void *), void *arg );
		
		bool IsGood( void ) { return !( sockfd < 0 ); };
		
		void Close() { close( sockfd ); };
		
	protected:
		
		int32_t sockfd;
		int32_t port;
		
		bool is_good;
		
		struct SocketTTL_s ttl;
		
};

class ClientSocket : public Socket
{
		friend void *SocketConnect( void *arg );
	public:
		ClientSocket( const char *host, const int32_t port );
		virtual ~ClientSocket();
		
		/*Socket &operator Socket( void ) const
		{
			return 
		}*/
		
		bool Connect( const int32_t retrys );
		
	private:
		
		const char *host;
		
};

class ServerSocket : public Socket
{
	public:
		ServerSocket( const int32_t port );
		virtual ~ServerSocket();
		
		Socket Accept( void ) const;
		
	private:
		
		
};

#endif /* SOCKET_H_ */
