/*
 * Hash.cpp
 *
 *  Created on: 16/07/2009
 *      Author: Peter S. Balling
 */

#include "Hash.h"

Hash::Hash(const Hash &x)
{
	memcpy(hash8_u, x.hash8_u, HASH8_SIZE);
}

Hash::Hash(const Password &pwd)
{
	SetPassword(pwd);
}

void Hash::Print( void ) const
{
	for ( uint32_t i = 0; i < HASH64_SIZE; i++ )
		 cout << hash64_u[i] << " ";
	cout << endl;
	cout << endl;
}
