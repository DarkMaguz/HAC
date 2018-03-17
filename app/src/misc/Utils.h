/*
 * Utils.h
 *
 *  Created on: 18-11-2008
 *      Author: Peter S. Balling
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <ctime>

using namespace std;

#ifdef __linux__

#define DIR_CHAR '/'

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#endif

#ifndef __linux__
#define DIR_CHAR '\\'
#endif

union raw_type
{
	char type1[8];
	long long type2;
};

bool mkfile( const char *path, bool overwrite = false );

bool makedir( const char *path, bool overwrite = false );
bool removedir( string path );
bool file_exists( const char *path );
bool is_file( const char *path );
bool is_dir( const char *path );
bool get_dir_content( const char *path, vector<string> &list );
const char *get_cwd( void );
unsigned long long get_file_size( const char *path );

bool copyfile( const char *src, const char *dst );

int spc_email_isvalid( const char *address );

string str_date( void );

#endif /* UTILS_H_ */
