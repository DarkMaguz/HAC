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
	Set<uint32_t> C;
	
	uint32_t size = HASH8_SIZE;
	uint32_t t1;
	
	//const password_t *pwd_ptr = pwd.Get1();
	const uint32_t *pwd_ptr = pwd.Get1();
	
	for (uint32_t i = 0; i < HASH8_SIZE ; i++)
	{
		t1 = C.At((pwd_ptr[i % pwd.Size()] * ((i / pwd.Size()) + 1)) % size);
		size--;
		hash8_u[i] = t1;
		
		C.Erase(t1);
	}
}

/*void Hash::operator =( const Hash &x )
 {
 
 memcpy( hash8_u, x.hash8_u, HASH8_SIZE );
 
 }

 void Hash::operator =( const Password &pwd )
 {
 
 Set<int> C;
 
 int size = HASH8_SIZE;
 int t1;
 
 for ( int i = 0; i < HASH8_SIZE; i++ )
 {
 
 t1 = C.At( ( pwd.At( i % pwd.Size() ) * ( ( i / pwd.Size() ) + 1 ) ) % size );
 size--;
 
 hash8_u[i] = t1;
 
 C.Erase( t1 );
 
 }
 
 }*/

//bool Hash::operator ==( const Hash &x ) const
//{
//	
//	/*for ( int i = 0; i < HASH8_SIZE; i++ )
//		if ( hash8_u[i] != x.hash8_u[i] )
//			return false;*/
//	
//	/*for ( int i = 0; i < HASH32_SIZE; i++ )
//		if ( hash32_u[i] != x.hash32_u[i] )
//			return false;*/
//	
//	for ( int i = 0; i < HASH64_SIZE; i++ )
//		if ( hash64_u[i] != x.hash64_u[i] )
//			return false;
//	
//	return true;
//	
//}
//bool Hash::operator <( const Hash &x ) const
//{
//	
//	/*for ( int i = 0; i < HASH8_SIZE; i++ )
//		if ( hash8_u[i] != x.hash8_u[i] )
//			return hash8_u[i] < x.hash8_u[i];*/
//	
//	/*for ( int i = 0; i < HASH32_SIZE; i++ )
//		if ( hash32_u[i] != x.hash32_u[i] )
//			return hash32_u[i] < x.hash32_u[i];*/
//	
//	for ( int i = 0; i < HASH64_SIZE; i++ )
//		if ( hash64_u[i] != x.hash64_u[i] )
//			return hash64_u[i] < x.hash64_u[i];
//	
//	return false;
//	
//}
//
//bool Hash::operator >( const Hash &x ) const
//{
//	
//	/*for ( int i = 0; i < HASH8_SIZE; i++ )
//		if ( hash8_u[i] != x.hash8_u[i] )
//			return hash8_u[i] > x.hash8_u[i];*/
//	
//	/*for ( int i = 0; i < HASH32_SIZE; i++ )
//		if ( hash32_u[i] != x.hash32_u[i] )
//			return hash32_u[i] > x.hash32_u[i];*/
//	
//	for ( int i = 0; i < HASH64_SIZE; i++ )
//		if ( hash64_u[i] != x.hash64_u[i] )
//			return hash64_u[i] > x.hash64_u[i];
//	
//	return false;
//	
//}
//
//bool Hash::operator >=( const Hash &x ) const
//{
//	
//	/*for ( int i = 0; i < HASH8_SIZE; i++ )
//		if ( hash8_u[i] < x.hash8_u[i] )
//			return false;*/
//	
//	/*for ( int i = 0; i < HASH32_SIZE; i++ )
//		if ( hash32_u[i] < x.hash32_u[i] )
//			return false;*/
//	
//	for ( int i = 0; i < HASH64_SIZE; i++ )
//		if ( hash64_u[i] > x.hash64_u[i] )
//			return true;
//		else if ( hash64_u[i] != x.hash64_u[i])
//			return false;
//	
//	return true;
//	
//}
