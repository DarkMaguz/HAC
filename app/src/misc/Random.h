/*
 * Random.h
 *
 *  Created on: 02-01-2009
 *      Author: Peter S. Balling
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include <cstdio>
using namespace std;

template <class Type>
void GetRnd( Type &rand )
{
	FILE* rand_src;
	
	rand_src = fopen( "/dev/urandom", "r" );
	
	fread( &rand, sizeof(Type), 1, rand_src );
	
	fclose( rand_src );
	
}

inline void GetRndStr( void *rand, size_t size )
{
	
	FILE* rand_src;
	
	rand_src = fopen( "/dev/urandom", "r" );
	
	fread( rand, 1, size, rand_src );
	
	fclose( rand_src );
	
}

/*template <class Type>
void GetRndStr( Type *rand, size_t size )
{
	
	FILE* rand_src;
	
	rand_src = fopen( "/dev/urandom", "r" );
	
	fread( &rand, sizeof(Type), size, rand_src );
	
	fclose( rand_src );
	
}*/

template <class Type>
class Random
{
	public:
		Random( unsigned long long i ) :
			it( i )
		{
			
			r = new Type[it];
			
			rand_src = fopen( "/dev/urandom", "r" );
			
			fread( r, sizeof(Type), it, rand_src );
			
			fclose( rand_src );
			
		}
		
		virtual ~Random()
		{
			delete []r;
		}
		
		inline Type Rnd( Type n )
		{
			
			it--;
			
			return ( r[it] % (n+1) );
			
		}
		
		/*inline Type Rnd2( void )
		{
			
			it--;
			
			return r[it];
			
		}*/
		
	protected:
		
	private:
		
		FILE* rand_src;
		
		unsigned long long it;
		
		Type *r;
		
};

#endif /* RANDOM_H_ */
