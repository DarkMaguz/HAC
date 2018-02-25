/*
 * NetStream.cpp
 *
 *  Created on: 24/07/2009
 *      Author: Peter S. Balling
 */

#include "NetStream.h"

NetStream::NetStream( const Socket &sockfd ) :
	Socket( sockfd )
{
	
}

NetStream::~NetStream()
{
	
}

bool NetStream::Write( const char *data, const int32_t &len )
{
	
	if ( !WriteSize( reinterpret_cast<const char *>( &len ) ) )
		return false;
	
	int32_t write_count = write( sockfd, data, len );
	
	int32_t sleeptime = 0;
	
	while ( write_count < len )
	{
		
		int32_t res = write( sockfd, data + write_count, len - write_count );
		
		if ( res == -1 )
			return false;
		else if ( res == 0 )
		{
			if ( sleeptime == 3 )
				return false;
			sleeptime++;
			usleep( 1000000 * sleeptime );
		}
		else
		{
			write_count += res;
			sleeptime = 0;
		}
		
	}
	
	return true;
	
}

bool NetStream::Read( char *data, int32_t &len )
{
	
	if ( !ReadSize( reinterpret_cast<char *>( &len ) ) )
		return false;
	
	return Reader( data, len );
	
	/*int32_t read_count = read( sockfd, data, len );
	
	int32_t sleeptime = 0;
	
	while ( read_count < len )
	{
		
		int32_t res = read( sockfd, data + read_count, len - read_count );
		
		if ( res == -1 )
			return false;
		else if ( res == 0 )
		{
			if ( sleeptime == 3 )
				return false;
			sleeptime++;
			usleep( 1000000 * sleeptime );
		}
		else
		{
			read_count += res;
			sleeptime = 0;
		}
		
	}
	
	return true;*/
	
}

bool NetStream::Read( char *data, uint32_t &ulen )
{
	
	int32_t len;
	
	if ( !ReadSize( reinterpret_cast<char *>( &len ) ) )
		return false;
	
	ulen = len;
	
	return Reader( data, len );
	
	/*int32_t read_count = read( sockfd, data, len );
	
	int32_t sleeptime = 0;
	
	while ( read_count < len )
	{
		
		int32_t res = read( sockfd, data + read_count, len - read_count );
		
		if ( res == -1 )
			return false;
		else if ( res == 0 )
		{
			if ( sleeptime == 3 )
				return false;
			sleeptime++;
			usleep( 1000000 * sleeptime );
		}
		else
		{
			read_count += res;
			sleeptime = 0;
		}
		
	}
	
	return true;*/
	
}

bool NetStream::Read( char *data )
{
	
	int32_t len;
	
	if ( !ReadSize( reinterpret_cast<char *>( &len ) ) )
		return false;
	
	return Reader( data, len );
	
	/*int32_t read_count = read( sockfd, data, len );
	
	int32_t sleeptime = 0;
	
	while ( read_count < len )
	{
		
		int32_t res = read( sockfd, data + read_count, len - read_count );
		
		if ( res == -1 )
			return false;
		else if ( res == 0 )
		{
			if ( sleeptime == 3 )
				return false;
			sleeptime++;
			usleep( 1000000 * sleeptime );
		}
		else
		{
			read_count += res;
			sleeptime = 0;
		}
		
	}
	
	return true;*/
	
}

bool NetStream::Reader( char *data, const int32_t &len )
{
	
	int32_t read_count = read( sockfd, data, len );
	
	int32_t sleeptime = 0;
	
	while ( read_count < len )
	{
		
		int32_t res = read( sockfd, data + read_count, len - read_count );
		
		if ( res == -1 )
			return false;
		else if ( res == 0 )
		{
			if ( sleeptime == 3 )
				return false;
			sleeptime++;
			usleep( 1000000 * sleeptime );
		}
		else
		{
			read_count += res;
			sleeptime = 0;
		}
		
	}
	
	return true;
	
}

bool NetStream::WriteSize( const char *size )
{
	
	int32_t write_count = write( sockfd, size, 4 );
	
	int32_t sleeptime = 0;
	
	while ( write_count < 4 )
	{
		
		int32_t res = write( sockfd, size + write_count, 4 - write_count );
		
		if ( res == -1 )
			return false;
		else if ( res == 0 )
		{
			if ( sleeptime == 3 )
				return false;
			sleeptime++;
			usleep( 1000000 * sleeptime );
		}
		else
		{
			write_count += res;
			sleeptime = 0;
		}
		
	}
	
	return true;
	
}

bool NetStream::ReadSize( char *size )
{
	
	/*int32_t read_count = read( sockfd, size, 4 );
	
	int32_t sleeptime = 0;
	
	while ( read_count < 4 )
	{
		
		int32_t res = read( sockfd, size + read_count, 4 - read_count );
		
		if ( res == -1 )
			return false;
		else if ( res == 0 )
		{
			if ( sleeptime == 3 )
				return false;
			sleeptime++;
			usleep( 1000000 * sleeptime );
		}
		else
		{
			read_count += res;
			sleeptime = 0;
		}
		
	}
	
	return true;*/
	
	return Reader( size, 4 );
	
}
