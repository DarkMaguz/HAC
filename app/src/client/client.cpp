/*
 * main.cpp
 *
 *  Created on: 16/07/2009
 *      Author: Peter S. Balling
 */

#include "HAC.h"

#include <iostream>
#include <cmath>
#include <map>
#include <algorithm>
#include <csignal>
#include <cerrno>
using namespace std;

const char useage[] =
{
	"\n\tUsage: { hac } [ OPTION ]\n\n" \
	"\tOptions\t\t"			"Long options\t\t"				"Descriptions\n" \
	"\t -r USER EMAIL\t"	" --register USER EMAIL\t"	" Register a new user.\n" \
	"\t -a\t\t"				" --activate\t\t"					" Activate user.\n" \
	"\t -c\t\t"				" --configure\t\t"					" Run configuration.\n" \
	"\t -u USER\t"			" --user USER\t\t"				" Change user.\n" \
	"\t -b\t\t"				" --benchmark\t\t"				" Run benchmark test.\n" \
	"\t -d PATH\t"			" --debug PATH\t\t"				" Debug work unit.\n" \
	"\t -h\t\t"				" --help\t\t\t"						" This message.\n"
};

enum args
{
	cmd_0,
	cmd_r,
	cmd_a,
	cmd_c,
	cmd_u,
	cmd_b,
	cmd_d,
	cmd_h
};

//void *tester01( void * arg )
//{
//	
//	FILE *fd = (FILE *)arg;
//	
//	while ( 1 )
//	{
//		sleep( 1 );
//		cout << "kkk" << endl;
//		/*ungetc( '\n', fd );*/
//		fflush_unlocked( fd );
//	}
//	
//	return 0;
//	
//}

int main( int argc, char **argv )
{
	
	/*//Password tmp1( 10000 );
	
	for ( uint64_t i = 10000; i < 10100; i++ )
	{
		
		Password tmp1;//( i );
		tmp1 += i;
		
		if ( tmp1.Convert() != i )
		{
			cout << i << endl;
			cout << tmp1.Convert() << endl;
		}
		
		//tmp1++;
		
	}*/
	
	/*for ( uint64_t i = 0; i < 900000000; i++ )
	{
		
		Password tmp1( 1000000000 );
		Password tmp2( i );
		
		if ( tmp1.Sub( tmp2 ) != 1000000000 - i )
		{
			cout << tmp1.Sub( tmp2 ) << endl;
			cout << 1000000000 - i << endl;
			cout << i << endl << endl;
			break;
		}
		
	}*/
	
	/*Password tmp1( 10000 );
	Password tmp2( 6095 );
	
	cout << tmp1.Sub( tmp2 ) << endl;
	cout << 10000 - 6095 << endl;*/
	
	/*uint64_t i = 0;
	
	Password tmp1;
	
	tmp1.Print( cout );
	cout << endl;
	
	tmp1 += i;
	
	tmp1.Print( cout );
	cout << endl;*/
	
	/*int tmp1[] = { 5,8,4,6,1,2,7,9,3,0 };
	
	sort( tmp1, tmp1 + 11 );
	
	cout << tmp1[0];
	for ( int i = 1; i < 10; i++ )
	{
		cout << "," << tmp1[i];
	}
	
	cout << endl;*/
	
//	for ( uint64_t j = 100; j < 10000000; j++ )
//	{
//		Password2 tmp1( j );
//		Password2 tmp2( tmp1 );
//		
//		for ( uint64_t i = 1; i < 10000000; i++ )
//		{
//			
//			Password2 tmp3( tmp1 );
//			//Password2 tmp4( i );
//			tmp2++;
//			tmp3 += i;
//			
//			if ( tmp2 != tmp3 )
//			{
//				cout << "k " << i << "\t";
//				tmp2.Print( cout );
//				cout << " != ";
//				tmp3.Print( cout );
//				cout << endl;
//				cout << "k " << i << "\t";
//				cout << tmp2.Convert() << " != " << tmp3.Convert() << endl;
//				break;
//				/*tmp4.Print( cout );
//				cout << endl;*/
//			}
//			/*else
//			{
//				cout << "k " << i << "\t";
//				tmp2.Print( cout );
//				cout << " == ";
//				tmp3.Print( cout );
//				cout << endl;
//				cout << "k " << i << "\t";
//				cout << tmp2.Convert() << " == " << tmp3.Convert() << endl;
//				//tmp4.Print( cout );
//				//cout << endl;
//			}*/
//			
//			//if ( tmp3 != tmp2 )
//			//{
//			//	cout << "k " << i << endl;
//			//	return 0;
//			//}
//			
//		}
//	}
	
	/*while ( 1 )
	{
		//int c = getchar();
		char c;
		cin.get( &c, 1 );
		//cin >> c;
		cout << "-->" << c << endl;
		sleep( 2 );
	}*/
	
	/*FILE *fd = stdin;
	
	pthread_t tmp;
	
	pthread_create( &tmp, NULL, tester01, fd );
	
	char buffer[100];
	
	string c;
	while ( c.empty() )
	{
		
		fgets_unlocked( buffer, 2, fd );
		
		
		
		//cout << "-->" << buffer << endl;
		
		c.assign( buffer );
		
		//c = getchar_unlocked();
		
		cout << "-->" << c << endl;
	}*/
	
	/*
	while ( c == '\n' )
	{
		//usleep( 100000 );
		sleep( 1 );
		ungetc( c, stdin );
		c = getchar();
		
	}*/
	
	//return 0;
	
	map<string, args> arg_map;
	arg_map["-r"] = cmd_r;
	arg_map["--register"] = cmd_r;
	arg_map["-a"] = cmd_a;
	arg_map["--activate"] = cmd_a;
	arg_map["-c"] = cmd_c;
	arg_map["--configure"] = cmd_c;
	arg_map["-u"] = cmd_u;
	arg_map["--user"] = cmd_u;
	arg_map["-b"] = cmd_b;
	arg_map["--benchmark"] = cmd_b;
	arg_map["-d"] = cmd_d;
	arg_map["--debug"] = cmd_d;
	arg_map["-h"] = cmd_h;
	arg_map["--help"] = cmd_h;
	
	if ( argc > 4 )
	{
		cout << useage << endl;
		return 0;
	}
	
	if ( argc == 2 )
	{
		if ( arg_map[argv[1]] == cmd_h )
		{
			cout << useage << endl;
			return 0;
		}
	}
	
	cout << "Loading..." << endl;
	
	HAC client;
	
	if ( !client.cfg_ok )
		return 0;
	
	if ( argc != 1 )
	{
		
		switch ( arg_map[argv[1]] )
		{
			case cmd_r:
			{
				
				if ( argc < 3 )
				{
					cout << useage << endl;
					return 0;
				}
				
				client.user = argv[2];
				
				client.Register( argv[3] );
				
				break;
			}
			case cmd_a:
			{
				
				client.Activate();
				
				break;
			}
			case cmd_c:
			{
				
				client.Configure();
				
				break;
			}
			case cmd_u:
			{
				
				client.user = argv[2];
				client.SaveConfig();
				
				break;
			}
			case cmd_b:
			{
				
				cout << "Score: " << client.BenchMark() << endl;
				
				break;
			}
			case cmd_d:
			{
				
				client.Debug( argv[2] );
				
				break;
			}
			case cmd_h:
			{
				
				cout << useage << endl;
				return 0;
				
				break;
			}
			default:
				break;
		}
		
	}
	else
	{
		
		client.Start();
		
	}
	
	return 0;
	
}
