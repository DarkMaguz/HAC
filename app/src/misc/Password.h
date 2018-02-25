/*
 * Password.h
 *
 *  Created on: 24/06/2009
 *      Author: Peter S. Balling
 */

#ifndef PASSWORD_H_
#define PASSWORD_H_

#include "NetStream.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <ostream>
#include <istream>
#include <ios>
#include <cmath>
#include <algorithm>
using namespace std;

#define PASSWORD_SIZE (uint32_t)64

typedef uint32_t password_t[PASSWORD_SIZE ];
//typedef uint8_t password_char_t[PASSWORD_SIZE+1];

#ifndef getmax
#define getmax(a,b) a>b?a:b
#endif
#ifndef getmin
#define getmin(a,b) a<b?a:b 
#endif

class Password
{
	public:
		Password(const uint64_t x = 0);
		Password(const password_t &x, const uint32_t &size);
		Password(const Password &x);
		Password(const uint8_t *x);
		Password(const string &x);

		void operator =(uint64_t x);
		void operator =(const Password &x);

		friend ostream &operator <<(ostream &os, const Password &x);
		friend istream &operator >>(istream &is, Password &x);

		NetStream &operator <<(NetStream &ns) const
		{
			uint8_t *tmp = new uint8_t[size];
			
			for (uint32_t i = 0; i < size; i++)
				tmp[i] = password[i];
			
			ns.Write(reinterpret_cast<const char *>(tmp), size);
			
			delete[] tmp;
			
			return ns;
		}
		
		NetStream &operator >>(NetStream &ns)
		{
			uint8_t *tmp = new uint8_t[PASSWORD_SIZE ];
			
			ns.Read(reinterpret_cast<char *>(tmp), size);
			
			memset(password + size, 0, (PASSWORD_SIZE - size) * 4);
			
			for (uint32_t i = 0; i < size; i++)
				password[i] = tmp[i];
			
			delete[] tmp;
			
			return ns;
		}
		
		bool operator ==(const Password &x) const;
		bool operator !=(const Password &x) const;
		
		bool operator <(const Password &x) const;
		bool operator >(const Password &x) const;
		
		bool operator <=(const Password &x) const;
		bool operator >=(const Password &x) const;
		
		void operator +=(uint64_t x);
		//void operator +=( const Password &x );
		void operator ++(int32_t);
		
		bool Add(uint64_t x);
		//bool Add( const Password &x );
		uint64_t MaxAdd(void) const;
		
		uint64_t Sub(const Password &x) const;
		
		uint64_t Convert(void) const;
		
		const uint32_t *Get1(void) const;
		//const uint8_t *Get2( void ) const;
		
		const uint32_t Size(void) const;
		
		const int32_t Print(ostream &os) const;
		
		string String(void) const;
		
	private:
		password_t password;
		uint32_t size;
		
		mutable password_t pwd1;
		//mutable password_char_t pwd2;
		
		static const uint32_t alpha_list_size = 62;
		
};

ostream &operator <<(ostream &os, const Password &x);
istream &operator >>(istream &is, Password &x);

#endif /* PASSWORD_H_ */
