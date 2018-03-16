/*
 * HashData.h
 *
 *  Created on: 16. mar. 2018
 *      Author: magnus
 */

#ifndef HASHDATA_H_
#define HASHDATA_H_

#include "Password.h"
#include "Hash.h"

#define HASHDATA_SIZE 264

struct HashData
{
	HashData() :
		id( 0 )
	{
	};
	
	HashData( const Password &pwd ) :
		hash( pwd ),
		id( 0 )
	{
	};
	
	HashData( const Password &pwd, const uint64_t &id ) :
		hash( pwd ),
		id( id )
	{
	};
	
	Hash hash;
	uint64_t id;
	
	inline bool operator ==( const HashData &x ) const
	{
		return hash == x.hash;
	}
	inline bool operator !=( const HashData &x ) const
	{
		return hash != x.hash;
	}
	inline bool operator <( const HashData &x ) const
	{
		return hash < x.hash;
	}
	inline bool operator >( const HashData &x ) const
	{
		return hash > x.hash;
	}
	inline bool operator >=( const HashData &x ) const
	{
		return hash >= x.hash;
	}
	
};



#endif /* HASHDATA_H_ */
