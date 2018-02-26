/*
 * hacd.cpp
 *
 *  Created on: 08/07/2009
 *      Author: Peter S. Balling
 */

#include "hacd.h"

void Client( User *user )
{
	
	int32_t choice;
	
	user->ns >> choice;
	user->server->log.Put( 0, user->id, ">>choice = %d.", choice );
	
	mysqlpp::Connection conn( false );
	conn.connect( DB_NAME, DB_HOST_NAME, DB_USER, DB_PASS );
	mysqlpp::Query query = conn.query();
	mysqlpp::StoreQueryResult res;
	
	while ( choice != 5 )
	{
		
		switch ( choice )
		{
			
			case 1: // Get data from client
			{
				
				uint32_t wu_id;
				user->ns >> wu_id;
				user->server->log.Put( 4, user->id, ">>wu_id = %u.", wu_id );
				
				uint64_t x_completed;
				user->ns >> x_completed;
				user->server->log.Put( 4, user->id, ">>x_completed = %U.", x_completed );
				
				query = conn.query();
				query << "SELECT * FROM workunit WHERE id = \"" << user->id << "\" AND wu_id = \"" << wu_id << "\" LIMIT 1;";
				pthread_mutex_lock( &user->server->mutex_workunit );
				res = query.store();
				if ( !res )
				{
					user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
				}
				
				if ( !res.empty() )
				{// Work unit is authenticated to this user
					
					user->ns << true;
					user->server->log.Put( 4, user->id, "<<bool = %s.", "true" );
					
					if ( (uint64_t)res[0]["x_size"] == x_completed )
					{
						
						query = conn.query();
						query << "DELETE FROM workunit WHERE id = \"" << user->id << "\" AND wu_id = \"" << wu_id << "\" LIMIT 1;";
						if ( !query.execute() )
						{
							user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
						}
						pthread_mutex_unlock( &user->server->mutex_workunit );
						
						query = conn.query();
						query << "INSERT INTO completed ( id, x_password, x_size, y_password, y_size, wu_id ) VALUES ( \""
							<< user->id << "\", \""
							<< mysqlpp::escape << res[0]["x_password"] << "\", \""
							<< x_completed << "\", \""
							<< mysqlpp::escape << res[0]["y_password"] << "\", \""
							<< res[0]["y_size"] << "\", \""
							<< wu_id << "\" );";
						if ( !query.execute() )
						{
							user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
						}
						
						query = conn.query();
						query << "UPDATE user SET score = score + " << (uint64_t)sqrt( (uint64_t)res[0]["x_size"] * (uint64_t)res[0]["y_size"] ) << ", "
							<< "wu_completed = wu_completed + 1, "
							<< "wu_inprogress = wu_inprogress - 1 "
							<< "WHERE id = \"" << user->id << "\" LIMIT 1;";
						if ( !query.execute() )
						{
							user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
						}
						
					}
					else
					{
						
						query = conn.query();
						query << "UPDATE workunit SET x_completed = \"" << x_completed << "\", "
							<< "time = \"" << (uint64_t)time( NULL ) << "\" "
							<< "WHERE id = \"" << user->id << "\" AND wu_id = \"" << wu_id << "\" LIMIT 1;";
						if ( !query.execute() )
						{
							user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
						}
						pthread_mutex_unlock( &user->server->mutex_workunit );
						
					}
					
				}
				else
				{// Work unit is not authenticated to this user
					
					pthread_mutex_unlock( &user->server->mutex_workunit );
					
					user->ns << false;
					user->server->log.Put( 4, user->id, "<<bool = %s.", "false" );
					
				}
				
				break;
			}
			case 2: // Send data to client
			{
				
				bool status = false;
				Password x_password;
				Password y_password;
				uint64_t x_size, x_completed, y_size;
				
				uint64_t y_max_size;
				user->ns >> y_max_size;
				user->server->log.Put( 5, user->id, ">>y_max_size = %U.", y_max_size );
				
				uint64_t benchmark;
				user->ns >> benchmark;
				user->server->log.Put( 5, user->id, ">>benchmark = %U.", benchmark );
				
				uint64_t the_time = time( NULL );
				
				query = conn.query();
				query << "UPDATE user SET wu_inprogress = wu_inprogress + 1, "
					<< "wu_total = wu_total + 1 "
					<< "WHERE id = \"" << user->id << "\" LIMIT 1;";
				pthread_mutex_lock( &user->server->mutex_user );
				if ( !query.execute() )
					user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
				
				query = conn.query();
				query << "SELECT wu_total FROM user WHERE id = \"" << user->id << "\" LIMIT 1;";
				res = query.store();
				pthread_mutex_unlock( &user->server->mutex_user );
				if ( !res )
					user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
				
				user->wu_total = res[0]["wu_total"];
				
				query = conn.query();
				query << "SELECT * FROM workunit WHERE time <= " << ( the_time + 2592000 ) << " AND y_size <= " << y_max_size << " ORDER BY y_size DESC LIMIT 1;";
				pthread_mutex_lock( &user->server->mutex_workunit );
				res = query.store();
				if ( !res )
					user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
				
				if ( !res.empty() )
				{// Found uncompleted work unit
					
					query = conn.query();
					query << "UPDATE workunit SET id = \"" << user->id << "\", "
						<< "time = \"" << the_time << "\", "
						<< "wu_id = \"" << user->wu_total << "\", "
						<< "benchmark = \"" <<benchmark << "\" "
						<< "WHERE id = \"" << res[0]["id"] << "\" AND wu_id = \"" << res[0]["wu_id"] << "\" LIMIT 1;";
					if ( !query.execute() )
						user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
					
					pthread_mutex_unlock( &user->server->mutex_workunit );
					
					query = conn.query();
					query << "INSERT INTO moved ( id_from, id_to, wu_from, wu_to ) VALUES ( \""
						<< res[0]["id"] << "\", \""
						<< user->id << "\", \""
						<< res[0]["wu_id"] << "\", \""
						<< user->wu_total << "\" );";
					
					if ( !query.execute() )
						user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
					
					status = true;
					x_password = reinterpret_cast<const uint8_t *>( res[0]["x_password"].c_str() );
					y_password = reinterpret_cast<const uint8_t *>( res[0]["y_password"].c_str() );
					x_size = res[0]["x_size"];
					x_completed = res[0]["x_completed"];
					y_size = res[0]["y_size"];
					
				}
				else
				{// Find a new work unit in bank
					
					pthread_mutex_unlock( &user->server->mutex_workunit );
					
					query = conn.query();
					query << "SELECT * FROM wubank WHERE y_size <= " << y_max_size << " AND open = 1 ORDER BY y_size DESC LIMIT 1;";
					pthread_mutex_lock( &user->server->mutex_wubank );
					res = query.store();
					if ( !res )
						user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
					
					x_size = benchmark * TARGET_WORK_TIME;
					
					if ( !res.empty() )
					{// Get new work unit from bank
						
						x_password = reinterpret_cast<const uint8_t *>( res[0]["x_password"].c_str() );
						
						if ( !x_password.Add( x_size ) )
						{
							
							x_password = reinterpret_cast<const uint8_t *>( res[0]["x_password"].c_str() );
							x_size = x_password.MaxAdd();
							
							query = conn.query();
							query << "UPDATE wubank SET open = 0 WHERE id = " << res[0]["id"] << " LIMIT 1;";
							if ( !query.execute() )
								user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
							
						}
						else
						{
							
							query = conn.query();
							query << "UPDATE wubank SET x_password = " << mysqlpp::quote << x_password.String() << " WHERE id = " << res[0]["id"] << " LIMIT 1;";
							if ( !query.execute() )
								user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
							
						}
						
						pthread_mutex_unlock( &user->server->mutex_wubank );
						
						status = true;
						y_password = reinterpret_cast<const uint8_t *>( res[0]["y_password"].c_str() );
						x_completed = 0;
						y_size = res[0]["y_size"];
						
					}
					else
					{// Bank does not contain any work units
						
						query = conn.query();
						query << "SELECT * FROM wubank ORDER BY id DESC LIMIT 1;";
						res = query.store();
						if ( !res )
							user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
						
						if ( !res.empty() )
						{
							
							y_password = reinterpret_cast<const uint8_t *>( res[0]["y_password"].c_str() );
							
							if ( !y_password.Add( y_max_size ) )
							{
								
								y_password = reinterpret_cast<const uint8_t *>( res[0]["y_password"].c_str() );
								y_max_size = y_password.MaxAdd();
								
							}
							
						}
						else
						{
							
							x_password = y_max_size;
							
						}
						
						query = conn.query();
						query << "INSERT INTO wubank ( y_password, y_size, x_password, open ) VALUES ( \""
							<< mysqlpp::escape << y_password.String() << "\", \""
							<< y_max_size << "\", \""
							<< mysqlpp::escape << x_password.String() << "\", \""
							<< true << "\" );";
						if ( !query.execute() )
							user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
						
						pthread_mutex_unlock( &user->server->mutex_wubank );
						
						status = true;
						x_completed = 0;
						y_size = y_max_size;
						
					}
					
					/*if ( x_password <= y_password )
					{
						
						Password x_tmp( x_password );
						x_tmp += x_size;
						
						if ( x_tmp >= y_password )
						{
							x_size -= x_tmp.Sub( y_password );
							x_size--;
						}
						
					}
					else
					{
						
						Password y_tmp( y_password );
						y_tmp += y_size;
						
						if ( x_password <= y_tmp )
						{
							x_size -= y_tmp.Sub( x_password );
							x_size--;
							x_password == y_tmp;
							x_password++;
						}
						
					}*/
					
					query = conn.query();
					query << "INSERT INTO workunit ( id, x_password, x_size, y_password, y_size, time, wu_id, benchmark ) VALUES ( \""
						<< user->id << "\", \""
						<< mysqlpp::escape << x_password.String() << "\", \""
						<< x_size << "\", \""
						<< mysqlpp::escape << y_password.String() << "\", \""
						<< y_size << "\", \""
						<< the_time << "\", \""
						<< user->wu_total << "\", \""
						<< benchmark << "\" );";
					if ( !query.execute() )
						user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
					
				}
				
				user->ns << status;
				user->server->log.Put( 5, user->id, "<<status = %s.", BOOL2ALPHA(status) );
				
				if ( status )
				{
					
					user->ns << x_password;
					user->server->log.Put( 5, user->id, "<<x_password = %P.", &x_password );
					
					user->ns << x_size;
					user->server->log.Put( 5, user->id, "<<x_size = %U.", x_size );
					
					user->ns << x_completed;
					user->server->log.Put( 5, user->id, "<<x_completed = %U.", x_completed );
					
					user->ns << y_password;
					user->server->log.Put( 5, user->id, "<<y_password = %P.", &y_password );
					
					user->ns << y_size;
					user->server->log.Put( 5, user->id, "<<y_size = %U.", y_size );
					
					user->ns << user->wu_total;
					user->server->log.Put( 5, user->id, "<<wu_total = %u.", user->wu_total );
					
				}
				
				break;
			}
			case 3: // Free work unit
			{
				
				uint32_t wu_id;
				user->ns >> wu_id;
				user->server->log.Put( 6, user->id, ">>wu_id = %u.", wu_id );
				
				uint64_t x_completed;
				user->ns >> x_completed;
				user->server->log.Put( 6, user->id, ">>x_completed = %U.", x_completed );
				
				query = conn.query();
				query << "SELECT * FROM workunit WHERE id = \"" << user->id << "\" AND wu_id = \"" << wu_id << "\" LIMIT 1;";
				pthread_mutex_lock( &user->server->mutex_workunit );
				res = query.store();
				if ( !res )
				{
					user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
				}
				
				if ( !res.empty() )
				{// Work unit is authenticated to this user
					
					user->ns << true;
					user->server->log.Put( 6, user->id, "<<bool = %s.", "true" );
					
					if ( (uint64_t)res[0]["x_size"] == x_completed )
					{
						
						query = conn.query();
						query << "DELETE FROM workunit WHERE id = \"" << user->id << "\" AND wu_id = \"" << wu_id << "\" LIMIT 1;";
						if ( !query.execute() )
						{
							user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
						}
						pthread_mutex_unlock( &user->server->mutex_workunit );
						
						query = conn.query();
						query << "INSERT INTO completed ( id, x_password, x_size, y_password, y_size, wu_id ) VALUES ( \""
							<< user->id << "\", \""
							<< mysqlpp::escape << res[0]["x_password"] << "\", \""
							<< x_completed << "\", \""
							<< mysqlpp::escape << res[0]["y_password"] << "\", \""
							<< res[0]["y_size"] << "\", \""
							<< wu_id << "\" );";
						if ( !query.execute() )
						{
							user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
						}
						
						query = conn.query();
						query << "UPDATE user SET score = score + " << (uint64_t)sqrt( (uint64_t)res[0]["x_size"] * (uint64_t)res[0]["y_size"] ) << ", "
							<< "wu_completed = wu_completed + 1, "
							<< "wu_inprogress = wu_inprogress - 1 "
							<< "WHERE id = \"" << user->id << "\" LIMIT 1;";
						if ( !query.execute() )
						{
							user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
						}
						
					}
					else
					{
						
						query = conn.query();
						query << "UPDATE workunit SET x_completed = \"" << x_completed << "\", "
							<< "time = \"" << 0 << "\" "
							<< "WHERE id = \"" << user->id << "\" AND wu_id = \"" << wu_id << "\" LIMIT 1;";
						if ( !query.execute() )
						{
							user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
						}
						pthread_mutex_unlock( &user->server->mutex_workunit );
						
					}
					
				}
				else
				{// Work unit is not authenticated to this user
					pthread_mutex_unlock( &user->server->mutex_workunit );
					user->ns << false;
					user->server->log.Put( 6, user->id, "<<bool = %s.", "false" );
				}
				
				break;
			}
			case 4: // Get collision from client
			{
				
				uint32_t wu_id;
				user->ns >> wu_id;
				user->server->log.Put( 7, user->id, ">>wu_id = %u.", wu_id );
				
				vector<struct Coordinates> collision;
				user->ns >> collision;
				user->server->log.Put( 7, user->id, ">>collision --> size = %U.", collision.size() );
				
				for ( vector<struct Coordinates>::iterator it = collision.begin(); it < collision.end(); it++ )
				{
					
					query = conn.query();
					query << "INSERT INTO collision ( id, wu_id, x_pos, y_pos ) VALUES ( \""
						<< user->id << "\", \""
						//<< (uint64_t)time( NULL ) << "\", \""
						<< wu_id << "\", \""
						<< (*it).x << "\", \""
						<< (*it).y << "\" );";
					
					if ( !query.execute() )
					{
						user->server->log.Put( 8, user->id, "query = %s.\ne = %s.", query.str().c_str(), query.error() );
					}
					
				}
				
				break;
			}
			default:
				break;
			
		}
		
		uint64_t seconds = time( NULL ) - user->sessionStart;
		
		if (seconds > 60)
		{
			user->server->log.Put( 0, user->id, "Client connection exceeded maximum allowed connection after %d seconds.", seconds );
			break;
		}
		
		user->ns >> choice;
		user->server->log.Put( 0, user->id, ">>choice = %d.", choice );
		
	}
	
}

void *Welcome( void *ud )
{
	
	//cout << "Connected" << endl;
	
	User *user( (User *)ud );
	
	user->ns >> user->id;
	user->server->log.Put( 0, user->id, "%s.", "Connected" );
	
	bool send_id = ( user->id == 0 );
	
	if ( send_id )
	{
		user->ns >> user->name;
		user->server->log.Put( 0, user->id, ">>name = %s.", user->name.c_str() );
	}
	
	int32_t choice = 0;
	user->ns >> choice;
	user->server->log.Put( 0, user->id, ">>choice = %d.", choice );
	
	switch ( choice )
	{
		
		case 1: // Create new user
		{
			
			user->ns >> user->email;
			user->server->log.Put( 1, user->id, ">>email = %s.", user->email.c_str() );
			int32_t res = 0;
			
			try
			{
				res = NewUser( user );
			}
			catch (const int& e)
			{
				std::cout << e << std::endl;
			}
			catch (const char *e)
			{
				std::cout << e << std::endl;
			}
			
			
			user->ns << res;
			user->server->log.Put( 1, user->id, "<<res = %d.", res );
			
			break;
		}
		case 2: // Activate user
		{
			
			user->ns >> user->validation;
			user->server->log.Put( 2, user->id, ">>validation = %s.", user->validation.c_str() );
			
			bool res = user->Activate();
			
			user->ns << res;
			user->server->log.Put( 2, user->id, "<<res = %d.", (int32_t)res );
			
			if ( send_id )
			{
				user->ns << user->id;
				user->server->log.Put( 2, user->id, "<<id = %u.", user->id );
			}
			
			break;
		}
		case 3: // Authenticate user
		{
			
			bool auth_ok = user->Get();
			
			if ( auth_ok && user->validation.empty() )
			{
				
				user->ns << true;
				user->server->log.Put( 3, user->id, "<<bool = %s.", "true" );
				
				if ( send_id )
				{
					user->ns << user->id;
					user->server->log.Put( 3, user->id, "<<id = %u.", user->id );
				}
				
				Client( user );
				
			}
			else
			{
				
				user->ns << false;
				user->server->log.Put( 3, user->id, "<<bool = %s.", "false" );
				
				if ( auth_ok )
				{
					user->ns << user->id;
					user->server->log.Put( 3, user->id, "<<id = %u.", user->id );
				}
				else
				{
					user->ns << (uint32_t)0;
					user->server->log.Put( 3, user->id, "<<id = %u.", 0 );
				}
				
			}
			
			break;
		}
		default:// Disconnect
			break;
		
	}
	
	//cout << "Disconnected" << endl;
	
	user->server->log.Put( 0, user->id, "%s.", "Disconnected" );
	
	user->ns.Close();
	
	return 0;
	
}

int32_t NewUser( User *user )
{
	static const string goodchars = "-_.";
	static const uint32_t alpha_list[] =
	{
		97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116,
		117, 118, 119, 120, 121, 122, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 65, 66, 67, 68,
		69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90
	};
	static const uint32_t alpha_list_size = 62;
	static const char *mail_address = "activation@darkmagus.dk";
	static const char *mail_subject = "Activate your HAC account";
	static const char *mail_server = "172.17.0.4";
	
	// Check email
	if ( !spc_email_isvalid( user->email.c_str() ) )
		return 4;
	uint16_t esign_pos = user->email.find( '@' );
	if ( gethostbyname( user->email.substr( esign_pos + 1 ).c_str() ) == NULL )
		return 4;
	
	// Check user
	uint16_t userlen = user->name.length();
	if ( userlen < 6 )
		return 5;
	for ( uint16_t i = 0; i < userlen; i++ )
		if ( !isalnum( user->name[i] ) )
		{
			if ( i == 0 || i == userlen - 1 )
				return 6;
			else if ( goodchars.find( user->name[i] ) == string::npos )
				return 6;
		}
	
	mysqlpp::Connection conn( false );
	conn.connect( DB_NAME, DB_HOST_NAME, DB_USER, DB_PASS );
	
	// Check if user name is free
	mysqlpp::Query query = conn.query( "SELECT * FROM user WHERE name = \"" + user->name + "\" LIMIT 1" );
	pthread_mutex_lock( &user->server->mutex_user );
	mysqlpp::StoreQueryResult res = query.store();
	if ( res.num_rows() != 0 )
	{
		pthread_mutex_unlock( &user->server->mutex_user );
		return 7;
	}
	
	// Check if email is free
	query = conn.query( "SELECT * FROM user WHERE email = \"" + user->email + "\" LIMIT 1" );
	res = query.store();
	if ( res.num_rows() != 0 )
	{
		pthread_mutex_unlock( &user->server->mutex_user );
		return 8;
	}
	
	// Generate validation code
	char *validation_code = new char[13];
	GetRndStr( validation_code, 12 );
	for ( int32_t i = 0; i < 12; i++ )
		validation_code[i] = alpha_list[(unsigned char)validation_code[i] % alpha_list_size];
	validation_code[12] = '\0';
	
	// Add user
	query = conn.query();
	query << "INSERT INTO user ( name, email, validation ) VALUES ( \"" 
		<< user->name << "\", \"" 
		<< user->email << "\", \"" 
		<< validation_code << "\" );";
	mysqlpp::SimpleResult sres = query.execute();
	
	pthread_mutex_unlock( &user->server->mutex_user );
	
	// Build mail
	string mail_data;
	mail_data.append( "Thank you for registering to become a member of HAC.\n" );
	mail_data.append( "\n" );
	mail_data.append( "When logging into the HAC client for the first time, you need to type in the following activation code when prompted:\n" );
	mail_data.append( "\n" );
	mail_data.append( "\t" );
	mail_data.append( validation_code );
	mail_data.append( "\n" );
	mail_data.append( "\n" );
	mail_data.append( "If you have any problems activating your account, please contact activation@hac.servebbs.org.\n" );
	mail_data.append( "\n" );
	mail_data.append( "HAC - Hash Algorithm Collider.\n" );
	
	// Send mail
	mailpp::send *myMailSender = new mailpp::send();
	myMailSender->sendMail( mail_server, mail_address, user->email.c_str(), mail_subject, mail_data.c_str() );
	
	return 0;
	
}

//int main( int argc, char **argv )
//{
//	
//	if ( argc > 1 )
//	{
//		string tmp1( argv[1] );
//		string tmp2( "TruncTables" );
//		if ( tmp1 == tmp2 )
//			TruncTables();
//		string tmp3( "CreateTables" );
//		if ( tmp1 == tmp3 )
//			CreateTables();
//	}
//	
//	struct ServerData server;
//	
//	pthread_t child;
//	pthread_attr_t pattr;
//	
//	ServerSocket ss( DEFAULT_SERVER_PORT );
//	
//	pthread_attr_init( &pattr );
//	pthread_attr_setdetachstate( &pattr, PTHREAD_CREATE_DETACHED );
//	
//	/*struct sigaction sa;
//	sa.sa_handler = Exit;
//	sigemptyset( &sa.sa_mask );
//	sigaction( SIGINT, &sa, NULL );*/
//	
//	while ( 1 )
//	{
//		
//		User *user = new User( ss.Accept(), &server );
//		
//		if ( user->ns.IsGood() )
//		{
//			pthread_create( &child, &pattr, Welcome, user );
//		}
//		else
//		{
//			delete []user;
//			cerr << "ERROR on accept" << endl;
//		}
//		
//	}
//	
//	return 0;
//	
//}

void TruncTables( void )
{
	
	mysqlpp::Connection conn( false );
	conn.connect( DB_NAME, DB_HOST_NAME, DB_USER, DB_PASS );
	
	mysqlpp::Query query = conn.query();
	query << "TRUNCATE TABLE collision;";
	if ( !query.execute() )
	{
		cout << query.error() << endl;
	}
	
	query = conn.query();
	query << "TRUNCATE TABLE completed;";
	if ( !query.execute() )
	{
		cout << query.error() << endl;
	}
	
	query = conn.query();
	query << "TRUNCATE TABLE moved;";
	if ( !query.execute() )
	{
		cout << query.error() << endl;
	}
	
	query = conn.query();
	query << "TRUNCATE TABLE user;";
	if ( !query.execute() )
	{
		cout << query.error() << endl;
	}
	
	query = conn.query();
	query << "TRUNCATE TABLE workunit;";
	if ( !query.execute() )
	{
		cout << query.error() << endl;
	}
	
	query = conn.query();
	query << "TRUNCATE TABLE wubank;";
	if ( !query.execute() )
	{
		cout << query.error() << endl;
	}
	
	exit( 0 );
	
}

void CreateTables( void )
{
	
	mysqlpp::Connection conn( false );
	conn.connect( DB_NAME, DB_HOST_NAME, DB_USER, DB_PASS );
	
	mysqlpp::Query query = conn.query();
	query << "CREATE TABLE  `hac01`.`collision` ("
		<< "`id` int(10) unsigned NOT NULL,"
		<< "`date` timestamp NOT NULL default CURRENT_TIMESTAMP,"
		<< "`wu_id` int(10) unsigned NOT NULL,"
		<< "`x_pos` bigint(20) unsigned NOT NULL,"
		<< "`y_pos` bigint(20) unsigned NOT NULL,"
		<< "PRIMARY KEY  (`id`)"
		<< ") ENGINE=MyISAM DEFAULT CHARSET=latin1;";
	if ( !query.execute() )
	{
		cout << query.error() << endl;
	}
	
	query = conn.query();
	query << "CREATE TABLE  `hac01`.`completed` ("
		<< "`id` int(10) unsigned NOT NULL,"
		<< "`x_password` tinyblob NOT NULL,"
		<< "`x_size` bigint(20) unsigned NOT NULL,"
		<< "`y_password` tinyblob NOT NULL,"
		<< "`y_size` bigint(20) unsigned NOT NULL COMMENT 'memsize = y_size*256',"
		<< "`wu_id` int(10) unsigned NOT NULL,"
		<< "PRIMARY KEY  (`id`,`wu_id`)"
		<< ") ENGINE=MyISAM DEFAULT CHARSET=latin1;";
	if ( !query.execute() )
	{
		cout << query.error() << endl;
	}
	
	query = conn.query();
	query << "CREATE TABLE  `hac01`.`moved` ("
		<< "`id_from` int(10) unsigned NOT NULL,"
		<< "`id_to` int(10) unsigned NOT NULL,"
		<< "`wu_from` int(10) unsigned NOT NULL,"
		<< "`wu_to` int(10) unsigned NOT NULL,"
		<< "PRIMARY KEY  (`id_from`)"
		<< ") ENGINE=MyISAM DEFAULT CHARSET=latin1;";
	if ( !query.execute() )
	{
		cout << query.error() << endl;
	}
	
	query = conn.query();
	query << "CREATE TABLE  `hac01`.`user` ("
		<< "`id` int(10) unsigned NOT NULL auto_increment,"
		<< "`name` tinytext NOT NULL,"
		<< "`email` tinytext NOT NULL,"
		<< "`score` bigint(20) unsigned default '0',"
		<< "`validation` char(12) NOT NULL,"
		<< "`wu_completed` int(10) unsigned default '0',"
		<< "`wu_inprogress` int(10) unsigned default '0',"
		<< "`wu_total` int(10) unsigned default '0',"
		<< "PRIMARY KEY  (`id`)"
		<< ") ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;";
	if ( !query.execute() )
	{
		cout << query.error() << endl;
	}
	
	query = conn.query();
	query << "CREATE TABLE  `hac01`.`workunit` ("
		<< "`id` int(10) unsigned NOT NULL,"
		<< "`x_password` tinyblob NOT NULL,"
		<< "`x_size` bigint(20) unsigned NOT NULL,"
		<< "`x_completed` bigint(20) unsigned default '0',"
		<< "`y_password` tinyblob NOT NULL,"
		<< "`y_size` bigint(20) unsigned NOT NULL COMMENT 'memsize = y_size*256',"
		<< "`time` bigint(20) unsigned NOT NULL,"
		<< "`wu_id` int(10) unsigned NOT NULL default '0',"
		<< "`benchmark` bigint(20) unsigned NOT NULL default '0',"
		<< "PRIMARY KEY  (`id`,`wu_id`)"
		<< ") ENGINE=MyISAM DEFAULT CHARSET=latin1;";
	if ( !query.execute() )
	{
		cout << query.error() << endl;
	}
	
	query = conn.query();
	query << "CREATE TABLE  `hac01`.`wubank` ("
		<< "`id` int(10) unsigned NOT NULL auto_increment,"
		<< "`y_password` tinyblob NOT NULL,"
		<< "`y_size` int(10) unsigned NOT NULL,"
		<< "`x_password` tinyblob NOT NULL,"
		<< "`open` tinyint(1) NOT NULL default '1',"
		<< "PRIMARY KEY  (`id`,`y_size`)"
		<< ") ENGINE=MyISAM AUTO_INCREMENT=10 DEFAULT CHARSET=latin1;";
	if ( !query.execute() )
	{
		cout << query.error() << endl;
	}
	
	exit( 0 );
	
}

/*CREATE TABLE  `hac01`.`collision` (
  `id` int(10) unsigned NOT NULL,
  `date` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `wu_id` int(10) unsigned NOT NULL,
  `x_pos` bigint(20) unsigned NOT NULL,
  `y_pos` bigint(20) unsigned NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`completed` (
  `id` int(10) unsigned NOT NULL,
  `x_password` tinyblob NOT NULL,
  `x_size` bigint(20) unsigned NOT NULL,
  `y_password` tinyblob NOT NULL,
  `y_size` bigint(20) unsigned NOT NULL COMMENT 'memsize = y_size*256',
  `wu_id` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`id`,`wu_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`moved` (
  `id_from` int(10) unsigned NOT NULL,
  `id_to` int(10) unsigned NOT NULL,
  `wu_from` int(10) unsigned NOT NULL,
  `wu_to` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`id_from`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`user` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `name` tinytext NOT NULL,
  `email` tinytext NOT NULL,
  `score` bigint(20) unsigned default '0',
  `validation` char(12) NOT NULL,
  `wu_completed` int(10) unsigned default '0',
  `wu_inprogress` int(10) unsigned default '0',
  `wu_total` int(10) unsigned default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`workunit` (
  `id` int(10) unsigned NOT NULL,
  `x_password` tinyblob NOT NULL,
  `x_size` bigint(20) unsigned NOT NULL,
  `x_completed` bigint(20) unsigned default '0',
  `y_password` tinyblob NOT NULL,
  `y_size` bigint(20) unsigned NOT NULL COMMENT 'memsize = y_size*256',
  `time` bigint(20) unsigned NOT NULL,
  `wu_id` int(10) unsigned NOT NULL default '0',
  `benchmark` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`,`wu_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1

CREATE TABLE  `hac01`.`wubank` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `y_password` tinyblob NOT NULL,
  `y_size` int(10) unsigned NOT NULL,
  `x_password` tinyblob NOT NULL,
  `open` tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (`id`,`y_size`)
) ENGINE=MyISAM AUTO_INCREMENT=10 DEFAULT CHARSET=latin1;*/

/*CREATE TABLE  `hac01`.`collision` (
  `id` int(10) unsigned NOT NULL,
  `date` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `wu_id` int(10) unsigned NOT NULL,
  `x_pos` bigint(20) unsigned NOT NULL,
  `y_pos` bigint(20) unsigned NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`completed` (
  `id` int(10) unsigned NOT NULL,
  `x_password` tinyblob NOT NULL,
  `x_size` bigint(20) unsigned NOT NULL,
  `y_password` tinyblob NOT NULL,
  `y_size` bigint(20) unsigned NOT NULL COMMENT 'memsize = y_size*256',
  `wu_id` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`id`,`wu_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`moved` (
  `id_from` int(10) unsigned NOT NULL,
  `id_to` int(10) unsigned NOT NULL,
  `wu_from` int(10) unsigned NOT NULL,
  `wu_to` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`id_from`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`user` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `name` tinytext NOT NULL,
  `email` tinytext NOT NULL,
  `score` bigint(20) unsigned default NULL,
  `validation` char(12) NOT NULL,
  `wu_completed` int(10) unsigned default '0',
  `wu_inprogress` int(10) unsigned default '0',
  `wu_total` int(10) unsigned default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`workunit` (
  `id` int(10) unsigned NOT NULL,
  `x_password` tinyblob NOT NULL,
  `x_size` bigint(20) unsigned NOT NULL,
  `x_completed` bigint(20) unsigned default NULL,
  `y_password` tinyblob NOT NULL,
  `y_size` bigint(20) unsigned NOT NULL COMMENT 'memsize = y_size*256',
  `time` bigint(20) unsigned NOT NULL,
  `wu_id` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`,`wu_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`wubank` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `y_password` tinyblob NOT NULL,
  `y_size` int(10) unsigned NOT NULL,
  `x_password` tinyblob NOT NULL,
  `open` tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (`id`,`y_size`)
) ENGINE=MyISAM AUTO_INCREMENT=10 DEFAULT CHARSET=latin1;*/

/*void TruncTable( void )
{
	
	mysqlpp::Connection conn( false );
	
	if ( conn.connect( "hash02", "localhost", "hashtest02", DB_PASS ) )
	{
		
		mysqlpp::Query query = conn.query();
		mysqlpp::SimpleResult sres;
		
		query << "ALTER TABLE hash ENGINE=MyISAM;";
		sres = query.execute();
		
		if ( !sres )
		{
			cerr << "Failed: ALTER TABLE hash ENGINE=MyISAM;" << query.error() << endl;
			return;
		}
		
		query = conn.query();
		
		query << "TRUNCATE TABLE hash;";
		sres = query.execute();
		
		if ( !sres )
		{
			cerr << "Failed: TRUNCATE TABLE hash;" << query.error() << endl;
			return;
		}
		
		query = conn.query();
		
		query << "ALTER TABLE hash ENGINE=ARCHIVE;";
		sres = query.execute();
		
		if ( !sres )
		{
			cerr << "Failed: ALTER TABLE hash ENGINE=ARCHIVE;" << query.error() << endl;
			return;
		}
		
	}
	else
	{
		cerr << "DB connection failed: " << conn.error() << endl;
	}
	
}

void BuildTables( void )
{
	
	mysqlpp::Connection conn( false );
	
	if ( conn.connect( "hash02", "localhost", "hashtest02", DB_PASS ) )
	{
		
		for ( int32_t i = 0; i < 256; i++ )
		{
			
			mysqlpp::Query query = conn.query();
			
			query << "CREATE TABLE hash" << i << " ("
				<< "`hash` tinyblob NOT NULL,"
				<< "`password` tinyblob NOT NULL,"
				<< "`id` int(10) unsigned NOT NULL"
				<< ") ENGINE=ARCHIVE DEFAULT CHARSET=latin1 COLLATE=latin1_danish_ci PACK_KEYS=1 ROW_FORMAT=COMPRESSED;";
			
			mysqlpp::SimpleResult sres = query.execute();
			
			if ( !sres )
				cerr << "Failed to create hash table: " << query.error() << endl;
			
		}
		
	}
	else
	{
		cerr << "DB connection failed: " << conn.error() << endl;
	}
	
}*/
