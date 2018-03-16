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

		NetStream &operator <<(NetStream &ns) const;
		NetStream &operator >>(NetStream &ns);
		
		bool operator ==(const Password &x) const;
		bool operator !=(const Password &x) const;
		
		bool operator <(const Password &x) const;
		bool operator >(const Password &x) const;
		
		bool operator <=(const Password &x) const;
		bool operator >=(const Password &x) const;
		
		void operator +=(uint64_t x);
		void operator ++(int32_t);
		
		bool Add(uint64_t x);
		uint64_t MaxAdd(void) const;
		
		uint64_t Sub(const Password &x) const;
		
		uint64_t Convert(void) const;
		
		const uint32_t *Get(void) const;
		
		const uint32_t Size(void) const;
		
		const int32_t Print(ostream &os) const;
		
		string String(void) const;
		
	private:
		password_t password;
		uint32_t size;
		
		mutable password_t pwd1;
		static const uint32_t alpha_list_size = 62;
};

ostream &operator <<(ostream &os, const Password &x);
istream &operator >>(istream &is, Password &x);

#endif /* PASSWORD_H_ */
