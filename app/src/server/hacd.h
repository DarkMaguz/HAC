/*
 * hacd.h
 *
 *  Created on: 08/07/2009
 *      Author: Peter S. Balling
 */

#ifndef HACD_H_
#define HACD_H_

#include "Log.h"
#include "Hash.h"
#include "Random.h"
#include "Password.h"
#include "NetStream.h"
#include "Utils.h"

#include "mailpp.h"

#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <csignal>
using namespace std;

#include <pthread.h>

#include <time.h>

#include <mysql/mysql.h>
#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>
//#include <mysql++.h>
//#include <ssqls.h>

#define DEFAULT_SERVER_PORT 4337
#define SERVER_HOST_NAME "localhost"

#define DB_HOST_NAME "hacdb"
#define DB_NAME "hac01"
#define DB_USER "hac"
#define DB_PASS "cah"

#define TARGET_WORK_TIME 1200 // Seconds of work in a WU

#define BOOL2ALPHA( x ) x ? "true" : "false"

/*sql_create_5( stock_completed, 1, 5,
		mysqlpp::sql_int_unsigned, id,
		mysqlpp::sql_timestamp, date,
		mysqlpp::sql_int_unsigned, wu_id,
		mysqlpp::sql_bigint_unsigned, x_pos,
		mysqlpp::sql_bigint_unsigned, y_pos )

sql_create_6( stock_completed, 1, 6,
		mysqlpp::sql_int_unsigned, id,
		mysqlpp::sql_tinyblob, x_password,
		mysqlpp::sql_bigint_unsigned, x_size,
		mysqlpp::sql_tinyblob, y_password,
		mysqlpp::sql_bigint_unsigned, y_size,
		mysqlpp::sql_int_unsigned, wu_id )

sql_create_4( stock_moved, 1, 4,
		mysqlpp::sql_int_unsigned, id_from,
		mysqlpp::sql_int_unsigned, id_to,
		mysqlpp::sql_int_unsigned, wu_from,
		mysqlpp::sql_int_unsigned, wu_to )

sql_create_8( stock_user, 1, 8,
		mysqlpp::sql_int_unsigned, id,
		mysqlpp::sql_tinytext, name,
		mysqlpp::sql_tinytext, email,
		mysqlpp::sql_bigint_unsigned, score,
		mysqlpp::sql_varchar, validation,
		mysqlpp::sql_int_unsigned, wu_completed,
		mysqlpp::sql_int_unsigned, wu_inprogress,
		mysqlpp::sql_int_unsigned, wu_total )

sql_create_8( stock_workunit, 1, 8,
		mysqlpp::sql_int_unsigned, id,
		mysqlpp::sql_tinyblob, x_password,
		mysqlpp::sql_bigint_unsigned, x_size,
		mysqlpp::sql_bigint_unsigned, x_completed,
		mysqlpp::sql_tinyblob, y_password,
		mysqlpp::sql_bigint_unsigned, y_size,
		mysqlpp::sql_bigint_unsigned, time,
		mysqlpp::sql_int_unsigned, wu_id,
		mysqlpp::sql_bigint_unsigned, benchmark )
		
sql_create_5( stock_wubank, 1, 5,
		mysqlpp::sql_int_unsigned, id,
		mysqlpp::sql_tinyblob, y_password,
		mysqlpp::sql_bigint_unsigned, y_size,
		mysqlpp::sql_tinyblob, x_password,
		mysqlpp::sql_bool, open )*/

struct Coordinates
{
	
	Coordinates( const uint64_t &x = 0, const uint64_t &y = 0 ) : x( x ), y( y ) { }
	uint64_t x;
	uint64_t y;
	
};

struct ServerData
{
	
	Log log;
	
	pthread_mutex_t mutex_user;
	pthread_mutex_t mutex_workunit;
	pthread_mutex_t mutex_collision;
	pthread_mutex_t mutex_wubank;
	
	ServerData() :
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
	
	virtual ~ServerData()
	{
		pthread_mutex_destroy( &mutex_user );
		pthread_mutex_destroy( &mutex_workunit );
		pthread_mutex_destroy( &mutex_collision );
		pthread_mutex_destroy( &mutex_wubank );
	}
	
};

struct UserData
{
	
	NetStream ns;
	
	struct ServerData *server;
	
	uint32_t id;
	string name;
	string email;
	uint64_t score;
	string validation;
	uint32_t wu_completed;
	uint32_t wu_inprogress;
	uint32_t wu_total;
	
	UserData( Socket sock, ServerData *server ) :
		ns( sock ),
		server( server ),
		id( 0 ),
		score( 0 ),
		wu_completed( 0 ),
		wu_inprogress( 0 ),
		wu_total( 0 )
	{
		
	}
	
	/*virtual ~UserData()
	{
		
	}
	
	private:
	
	UserData( const UserData &x ) :
		ns( x.ns ),
		server( x.server ),
		id( x.id ),
		name( x.name ),
		email( x.email ),
		score( x.score ),
		validation( x.validation ),
		wu_completed( x.wu_completed ),
		wu_inprogress( x.wu_inprogress ),
		wu_total( x.wu_total )
	{
		
	}
	
	UserData &operator =( const UserData &x )
	{
		
		id = x.id;
		ns = x.ns;
		server = x.server;
		name = x.name;
		email = x.email;
		score = x.score;
		validation = x.validation;
		wu_completed = x.wu_completed;
		wu_inprogress = x.wu_inprogress;
		wu_total = x.wu_total;
		
		return *this;
		
	}
	
	public:*/
	
	bool Activate( void )
	{
		
		if ( validation.length() != 12 )
			return false;
		
		if ( !name.empty() || id != 0 )
		{
			
			mysqlpp::Connection conn( false );
			conn.connect( DB_NAME, DB_HOST_NAME, DB_USER, DB_PASS );
			
			mysqlpp::Query query = conn.query();
			
			if ( id != 0 )
				query << "SELECT * FROM user WHERE id = \"" << id << "\" LIMIT 1;";
			else
				query << "SELECT * FROM user WHERE name = \"" << name << "\" LIMIT 1;";
			
			mysqlpp::Row row = query.use().fetch_row();
			
			if ( !row.empty() )
			{
				
				id = row["id"];
				row["name"].to_string( name );
				
				string val;
				row["validation"].to_string( val );
				
				if ( val == validation )
				{
					query = conn.query();
					query << "UPDATE user SET validation = \"\" WHERE id = \"" << id << "\" LIMIT 1;";
					query.execute();
						
					validation.clear();
					
					return true;
					
				}
				else
				{
					return false;
				}
				
			}
			
		}
		
		id = 0;
		
		return false;
		
	}
	
	bool Get( void )
	{
		
		if ( !name.empty() || id != 0 )
		{
			
			mysqlpp::Connection conn( false );
			conn.connect( DB_NAME, DB_HOST_NAME, DB_USER, DB_PASS );
			
			mysqlpp::Query query = conn.query();
			
			if ( id != 0 )
				query << "SELECT * FROM user WHERE id = \"" << id << "\" LIMIT 1;";
			else
				query << "SELECT * FROM user WHERE name = \"" << name << "\" LIMIT 1;";
			
			mysqlpp::StoreQueryResult res = query.store();
			if ( !res )
			{
				server->log.Put( 8, id, "e = %s.", query.error() );
			}
			
			if ( !res.empty() )
			{
				
				id = res[0]["id"];
				res[0]["name"].to_string( name );
				res[0]["email"].to_string( email );
				score = res[0]["score"];
				//memcpy( &score, res[0]["score"].c_str(), 8 );
				res[0]["validation"].to_string( validation );
				wu_completed = res[0]["wu_completed"];
				wu_inprogress = res[0]["wu_inprogress"];
				wu_total = res[0]["wu_total"];
				return true;
				
			}
			
		}
		
		return false;
		
	}
	
};

void Client( struct UserData *user );
void *Welcome( void *cs );

int32_t NewUser( struct UserData *user );

void TruncTables( void );
void CreateTables( void );

#endif /* HACD_H_ */
