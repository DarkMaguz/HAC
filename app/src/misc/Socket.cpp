/*
 * Socket.cpp
 *
 *  Created on: 24/07/2009
 *      Author: Peter S. Balling
 */

#include "Socket.h"

void *SocketConnect( void *arg )
{
	
	ClientSocket *sock = (ClientSocket *)arg;
	
	struct sockaddr_in addr;
	struct hostent *server = gethostbyname( sock->host );
	if ( server == NULL )
	{
		return 0;
	}
	
	sock->sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sock->sockfd < 0 )
	{
		return 0;
	}
	
	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family = AF_INET;
	addr.sin_port = htons( sock->port );
	memcpy( &addr.sin_addr.s_addr, server->h_addr, server->h_length );
	
	if ( connect( sock->sockfd, (struct sockaddr *)&addr, sizeof( addr ) ) < 0 )
	{
		
	}
	else
	{
		sock->is_good = true;
	}
	
	pthread_exit( 0 );
	
	return 0;
	
}

void *SocketTTL( void *arg )
{
	
	Socket *sock = ( Socket *)arg;
	
	if ( sock->ttl.seconds )
		sleep( sock->ttl.seconds );
	
	if ( sock->ttl.microseconds )
		usleep( sock->ttl.microseconds );
	
	sock->Close();
	
	if ( sock->ttl.stop_routine != NULL )
	{
		sock->ttl.stop_routine( sock->ttl.arg );
	}
	
	pthread_cancel( sock->ttl.parrent_threadid );
	
	return 0;
	
}

/*Socket::Socket( const Socket &x ) :
	sockfd( x.sockfd ),
	port( x.port ),
	is_good( x.is_good ),
	ttl( x.ttl )
{
	
}*/

Socket::Socket() :
	sockfd( 0 ),
	port( 0 ),
	is_good( false )
{
	
}

Socket::Socket( const int32_t sockfd, const int32_t port ) :
	sockfd( sockfd ),
	port( port ),
	is_good( !( sockfd < 0 ) )
{
	
}

void Socket::SetTTL( const int32_t &seconds, const int32_t &microseconds, void *(*stop_routine) (void *), void *arg )
{
	
	/*if ( ttl.is_set )
	{
		
	}
	else*/
	{
		//ttl = SocketTTL_s( seconds, microseconds, pthread_self() );
		ttl.seconds = seconds;
		ttl.microseconds = microseconds;
		ttl.parrent_threadid = pthread_self();
		ttl.stop_routine = stop_routine;
		ttl.arg = arg;
		
		pthread_attr_init( &ttl.pattr );
		pthread_attr_setdetachstate( &ttl.pattr, PTHREAD_CREATE_DETACHED );
		
		pthread_create( &ttl.threadid, &ttl.pattr, SocketTTL, this );
	}
	
}

Socket::~Socket()
{
	//close( sockfd );
}

ClientSocket::ClientSocket( const char *host, const int32_t port ) :
	Socket( -1, port ),
	host( host )
{
	
	//port = _port;
	
	/*int32_t retry = 0;
	pthread_t threadid;
	
	while ( retry < 3 && !is_good )
	{
		
		pthread_create( &threadid, NULL, SocketConnect, this );
		
		sleep( 3 );
		
		if ( !is_good )
		{
			pthread_cancel( threadid );
			retry++;
		}
		
	}*/
	
}

ClientSocket::~ClientSocket()
{
	//close( sockfd );
}

bool ClientSocket::Connect( const int32_t retrys )
{
	
	int32_t retry = 0;
	pthread_t threadid;
	pthread_attr_t pattr;
	pthread_attr_init( &pattr );
	pthread_attr_setdetachstate( &pattr, PTHREAD_CREATE_DETACHED );
	
	while ( retry < retrys && !is_good )
	{
		
		pthread_create( &threadid, &pattr, SocketConnect, this );
		
		sleep( 3 );
		
		if ( !is_good )
		{
			pthread_cancel( threadid );
			retry++;
		}
		
	}
	
	return is_good;
	
}

ServerSocket::ServerSocket( const int32_t port ) :
	Socket( socket( AF_INET, SOCK_STREAM, 0 ), port )
{
	
	struct sockaddr_in addr;
	
	//sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd < 0 )
	{
		perror( "socket" );
		abort();
	}
	
	int32_t on = 1;
	if ( ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on ) ) ) < 0 )
	{
		perror( "setsockoptions" );
		abort();
	}
	
	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = INADDR_ANY;
	
	if ( bind( sockfd, ( struct sockaddr* ) &addr, sizeof( addr ) ) != 0 )
	{
		perror( "bind" );
		abort();
	}
	
	if ( listen( sockfd, SOMAXCONN ) < 0 )
	{
		perror( "listen" );
		abort();
	}
	
}

ServerSocket::~ServerSocket()
{
	//close( sockfd );
}


Socket ServerSocket::Accept( void ) const
{
	
	int32_t new_sockfd = accept( sockfd, NULL, NULL );
	
	if ( new_sockfd < 0 )
	{
		cout << "Accept failed." << endl;
	}
	
	return Socket( new_sockfd, port );
	
}
