/*
 * Hash.h
 *
 *  Created on: 16/07/2009
 *      Author: Peter S. Balling
 */

#ifndef HASH_H_
#define HASH_H_

#include "Password.h"

#include <iostream>
#include <algorithm>
using namespace std;

#define HASH8_SIZE (uint32_t)256
#define HASH32_SIZE (uint32_t)64
#define HASH64_SIZE (uint32_t)32

typedef uint8_t hash8_t[HASH8_SIZE];
typedef uint32_t hash32_t[HASH32_SIZE];
typedef uint64_t hash64_t[HASH64_SIZE];

template<typename T> class Set
{
	typedef T Set_t[HASH8_SIZE];
	public:
		Set()
		{
			static const Set_t s_C =
			{
				0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,	30, 31, 32,
				33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
				63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92,
				93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117,
				118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
				141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163,
				164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186,
				187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
				210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232,
				233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
			};
			memcpy( data, s_C, HASH8_SIZE * sizeof( T ) );
		}
		
		inline void Erase( const T &at ) { data[at] = -1; }
		
		inline const T At( T at ) const
		{
			static const T npos = -1;
			for ( T i = 0; i <= at; i++ )
				if ( data[i] == npos )
					at++;
			return data[at];
		}
		
	private:
		Set_t data;
};

class Hash
{
	public:
		Hash() { };
		Hash( const Hash &x );
		Hash( const Password &pwd );
		virtual ~Hash() { };
		
		void Print( void )
		{
			for ( uint32_t i = 0; i < HASH64_SIZE; i++ )
			{
				 cout << hash64_u[i] << " ";
			}
			cout << endl;
			cout << endl;
		}
		
		//void operator =( const Hash &x );
		inline void operator =( const Password &pwd )
		{
			
			Set<uint32_t> C;
			
			uint32_t size = HASH8_SIZE;
			uint32_t t1;
			
			//const password_t *pwd_ptr = pwd.Get1();
			const uint32_t *pwd_ptr = pwd.Get1();
			
			for ( uint32_t i = 0; i < HASH8_SIZE; i++ )
			{
				
				t1 = C.At( ( pwd_ptr[ i % pwd.Size()] * ( ( i / pwd.Size() ) + 1 ) ) % size );
				size--;
				hash8_u[i] = t1;
				
				C.Erase( t1 );
				
			}
			
		}
		
		inline bool operator ==( const Hash &x ) const
		{
			for ( uint32_t i = 0; i < HASH64_SIZE; i++ )
				if ( hash64_u[i] != x.hash64_u[i] )
					return false;
			
			return true;
		}
		
		inline bool operator !=( const Hash &x ) const
		{
			return !this->operator ==( x );
		}
		
		bool operator <( const Hash &x ) const
		{
			for ( uint32_t i = 0; i < HASH64_SIZE; i++ )
				if ( hash64_u[i] != x.hash64_u[i] )
					return hash64_u[i] < x.hash64_u[i];
			
			return false;
		}
		bool operator >( const Hash &x ) const
		{
			for ( uint32_t i = 0; i < HASH64_SIZE; i++ )
				if ( hash64_u[i] != x.hash64_u[i] )
					return hash64_u[i] > x.hash64_u[i];
			
			return false;
		}
		bool operator >=( const Hash &x ) const
		{
			for ( uint32_t i = 0; i < HASH64_SIZE; i++ )
				if ( hash64_u[i] > x.hash64_u[i] )
					return true;
				else if ( hash64_u[i] != x.hash64_u[i])
					return false;
			
			return true;
		}
		
	private:
		
		union
		{
				hash8_t hash8_u;
				hash32_t hash32_u;
				hash64_t hash64_u;
		};
		
};

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

#endif /* HASH_H_ */
