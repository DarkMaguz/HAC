/*
 * User.cpp
 *
 *  Created on: 26. feb. 2018
 *      Author: magnus
 */

#include "User.h"

#include <mysql/mysql.h>
#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

#include <time.h>

User::User( Socket sock, Server *server ) :
	ns( sock ),
	sessionStart( time(NULL) ),
	server( server ),
	id( 0 ),
	score( 0 ),
	wu_completed( 0 ),
	wu_inprogress( 0 ),
	wu_total( 0 )
{
	
}

User::~User()
{
}

bool User::Activate( void )
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

bool User::Get( void )
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
