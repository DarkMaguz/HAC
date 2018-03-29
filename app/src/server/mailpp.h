/*
 * mailpp.h
 *
 *  Created on: 08/07/2009
 *      Author: Peter S. Balling
 */

#ifndef MAILPP_H_
#define MAILPP_H_

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
//#include <unistd.h>

using namespace std;

namespace mailpp
{
	class send
	{
		public:
			void sendMail( const char *, const char *, const char *, const char *, const char * );
			send();
			~send();
		private:
			void writeToSocket( const char *, bool, bool, bool );
			int *mailSocket;
			struct sockaddr_in *serverAddr;
			struct hostent *hostDB;
			int *len;
			char *buffer;
			string *strBuffer;
	};
	
	// Write and read to and/or from socket and/or check
	inline void send::writeToSocket( const char *s, bool writeThis, bool readThis, bool checkThis )
	{
		// Write
		if ( writeThis )
		{
			write( *mailSocket, s, strlen( s ) );
		}
		// Read
		if ( readThis )
		{
			*len = read( *mailSocket, buffer, BUFSIZ );
		}
		// Check
		if ( checkThis )
		{
			strBuffer->assign( *len, *buffer );
			if ( !strBuffer->find( "250", 0 ) || !strBuffer->find( "251", 0 ) || !strBuffer->find( "354", 0 ) )
			{
				close( *mailSocket );
				throw( *strBuffer );
			}
		}
	}
	
	// Usage sendMail( char* server, char* from , char* to, char* subject, char* message )
	inline void send::sendMail( const char *server, const char *from , const char *to, const char *subject, const char *message )
	{
		// Create socket
		mailSocket = new int;
		*mailSocket = socket( AF_INET, SOCK_STREAM, 0 );
		if ( *mailSocket < 0 )
		{
			throw( -1 );
		}
		serverAddr = new struct sockaddr_in;
		serverAddr->sin_family = AF_INET;
		hostDB = new struct hostent;
		// Set port 25
		serverAddr->sin_port = htons( 25 );
		len = new int;
		buffer = new char[BUFSIZ+1];
		strBuffer = new string;
		
		// Veryfi host
		*hostDB = *gethostbyname( server );
		if ( hostDB == (struct hostent *) 0 )
		{
			throw( -2 );
		}
		
		// Prepair connection to remote host
		memcpy( (char *)&serverAddr->sin_addr, (char *)hostDB->h_addr, hostDB->h_length );
		
		// Try to connect to remote host
		if ( connect( *mailSocket, (struct sockaddr *)serverAddr, sizeof( *serverAddr ) ) == -1 )
		{
			throw( -3 );
		}
		
		// Get connection message
		writeToSocket( (char *)"", false, true, false );
		
		// HELO
		writeToSocket( (char *)"HELO localhost\n", true, true, true );
		
		// MAIL
		writeToSocket( (char *)"MAIL FROM: ", true, false, false );
		writeToSocket( from, true, false, false );
		writeToSocket( (char *)"\n", true, true, true );
		
		// RCPT
		writeToSocket( (char *)"RCPT TO: ", true, false, false );
		writeToSocket( to, true, false, false );
		writeToSocket( (char *)"\n", true, true, true );
		
		// DATA start ---
		writeToSocket( (char *)"DATA\n", true, true, true );
		// SUBJECT
		writeToSocket( (char *)"SUBJECT: ", true, false, false );
		writeToSocket( subject, true, false, false );
		writeToSocket( (char *)"\n\n", true, false, false );
		writeToSocket( message, true, false, false );
		// DATA stop ---
		writeToSocket( (char *)"\n\n.\n", true, true, true );
		
		// QUIT
		writeToSocket( (char *)"QUIT\n", true, true, false );
		
		close( *mailSocket );
	}
	
	// Usage send *myMailSender = new send( char* server, char* from , char* to, char* subject, char* message )
	inline send::send()
	{
	}
	
	inline send::~send()
	{
		delete mailSocket;
		delete serverAddr;
		delete hostDB;
		delete len;
		delete []buffer;
		delete strBuffer;
	}
	
}  // namespace mailpp

#endif /*MAILPP_H_*/
