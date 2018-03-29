/*
 * hacd.h
 *
 *  Created on: 08/07/2009
 *      Author: Peter S. Balling
 */

#ifndef HACD_H_
#define HACD_H_

#include "Server.h"
#include "User.h"
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

#define TARGET_WORK_TIME 15 // Seconds of work in a WU

#define BOOL2ALPHA( x ) x ? "true" : "false"

void Client( User *user );
void *Welcome( void *cs );

int32_t NewUser( User *user );

void TruncTables( void );
void CreateTables( void );

#endif /* HACD_H_ */
