/*
 * main.cpp
 *
 *  Created on: 08/07/2009
 *      Author: Peter S. Balling
 */


#include "hacd.h"
#include "Server.h"

int main( int argc, char **argv )
{
	
	if ( argc > 1 )
	{
		string tmp1( argv[1] );
		string tmp2( "TruncTables" );
		if ( tmp1 == tmp2 )
			TruncTables();
		string tmp3( "CreateTables" );
		if ( tmp1 == tmp3 )
			CreateTables();
	}
	
	Server server;
	
	pthread_t child;
	pthread_attr_t pattr;
	
	pthread_attr_init( &pattr );
	pthread_attr_setdetachstate( &pattr, PTHREAD_CREATE_DETACHED );
	
	/*struct sigaction sa;
	sa.sa_handler = Exit;
	sigemptyset( &sa.sa_mask );
	sigaction( SIGINT, &sa, NULL );*/
	
	while ( 1 )
	{
		
		struct UserData *user = new struct UserData( server.Accept(), &server );
		
		if ( user->ns.IsGood() )
		{
			pthread_create( &child, &pattr, Welcome, user );
		}
		else
		{
			delete []user;
			cerr << "ERROR on accept" << endl;
		}
		
	}
	
	return 0;
	
}
