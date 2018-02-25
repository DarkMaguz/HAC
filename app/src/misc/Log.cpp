/*
 * Log.cpp
 *
 *  Created on: 02/08/2009
 *      Author: Peter S. Balling
 */

#include "Log.h"

Log::Log( const string &dir ) ://, const string &main_log_file ) :
	dir( dir )
{
	
	//struct LogFile *main = new struct LogFile( dir + main_log_file );
	
	//files.push_back( new struct LogFile( dir + main_log_file ) );
	
	makedir( dir.c_str() );
	
}

Log::~Log()
{
	
	for ( vector<struct LogFile *>::iterator it = files.begin(); it < files.end(); it++ )
		delete []*it;
	
}

void Log::AddFile( const string &log_file )
{
	
	mkfile( ( dir + "/" + log_file ).c_str() );
	
	//struct LogFile *logfile = new struct LogFile( dir + log_file );
	files.push_back( new struct LogFile( dir + "/" + log_file ) );
	
	/*mkfile( ( dir + "/" + log_file ).c_str() );
	
	struct LogFile *logfile = new struct LogFile( dir + log_file );
	
	logfile->fs << endl << "----------------------------------------------------------------------" << endl;
	files[file_id]->fs << time( NULL ) << ": " << "log start" << endl;
	logfile->fs << "----------------------------------------------------------------------" << endl << endl;
	
	files.push_back( logfile );*/
	
}

/*void Log::Put( const int &file_id, const uint32_t &user, const string &msg )
{
	
	pthread_mutex_lock( &files[file_id]->mutex_fs );
	*/
	/*time( &rawtime );
	timeinfo = localtime( &rawtime );*/
	
	//timeinfo->tm_hour << ":" << timeinfo->tm_mday << " " << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec << ":";
	
/**	files[file_id]->fs << time( NULL ) << " User " << user << ": " << msg << endl;
	
	pthread_mutex_unlock( &files[file_id]->mutex_fs );
	
}*/

void Log::Put( const int &file_id, const uint32_t &user, const string &format, ... )
{
	
	va_list ap;
	va_start( ap, format );
	
	size_t found = format.find( '%' );
	size_t last = 0;
	
	pthread_mutex_lock( &files[file_id]->mutex_fs );
	
	files[file_id]->fs << str_date() << " " << user << ": ";
	
#if DEBUG_LOG == 1
	cout << files[file_id]->path << ": ";
	cout << str_date() << " " << user << ": ";
#endif
	
	while ( found != string::npos )
	{
		
		files[file_id]->fs << format.substr( last, found - last );
#if DEBUG_LOG == 1
				cout << format.substr( last, found - last );
#endif
		found++;
		switch ( format[found] )
		{
			case 'd':
			{
				int32_t p = va_arg( ap, int32_t );
				files[file_id]->fs << p;
#if DEBUG_LOG == 1
				cout << p;
#endif
				break;
			}
			case 'D':
			{
				int64_t p = va_arg( ap, int64_t );
				files[file_id]->fs << p;
#if DEBUG_LOG == 1
				cout << p;
#endif
				break;
			}
			case 'u':
			{
				uint32_t p = va_arg( ap, uint32_t );
				files[file_id]->fs << p;
#if DEBUG_LOG == 1
				cout << p;
#endif
				break;
			}
			case 'U':
			{
				uint64_t p = va_arg( ap, uint64_t );
				files[file_id]->fs << p;
#if DEBUG_LOG == 1
				cout << p;
#endif
				break;
			}
			case 's':
			{
				char *p = va_arg( ap, char * );
				files[file_id]->fs << p;
#if DEBUG_LOG == 1
				cout << p;
#endif
				break;
			}
			case 'P':
			{
				Password *p = (Password *)va_arg( ap, void * );
				//files[file_id]->fs << p;
				p->Print( files[file_id]->fs );
#if DEBUG_LOG == 1
				//cout << p << " @ ";
				p->Print( cout );
#endif
				break;
			}
			default:
				break;
		}
		
		last = found + 1;
		
		found = format.find( '%', last );
		
	}
	
	files[file_id]->fs << format.substr( last ) << endl;
#if DEBUG_LOG == 1
	cout << format.substr( last ) << endl;
#endif
	
	pthread_mutex_unlock( &files[file_id]->mutex_fs );
	
}
