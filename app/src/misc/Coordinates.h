/*
 * Coordinates.h
 *
 *  Created on: 4. mar. 2018
 *      Author: Peter S. Balling
 */

#ifndef APP_SRC_MISC_COORDINATES_H_
#define APP_SRC_MISC_COORDINATES_H_

#include <stdint.h>

struct Coordinates
{
	
	Coordinates( const uint64_t &x = 0, const uint64_t &y = 0 ) : x( x ), y( y ) { }
	uint64_t x;
	uint64_t y;
		
};

#endif /* APP_SRC_MISC_COORDINATES_H_ */
