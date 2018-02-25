/*
 * Password.cpp
 *
 *  Created on: 24/06/2009
 *      Author: Peter S. Balling
 */

#include "Password.h"

static const int32_t alpha_list[] =
{
	97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116,
	117, 118, 119, 120, 121, 122, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 65, 66, 67, 68,
	69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90
};

static const int32_t *alpha_list_end = alpha_list + 62;

Password::Password(const uint64_t x)
{
	memset(password, 0, 256);
	*this = x;
}

Password::Password(const password_t &x, const uint32_t &size) :
	size(size)
{
	memcpy(password, x, size * 4);
	memset(password + size, 0, (PASSWORD_SIZE - size) * 4);
}

Password::Password(const Password &x) :
	size(x.size)
{
	memcpy(password, x.password, 256);
}

Password::Password(const uint8_t *x) :
	size(x[0])
{
	memset(password + size, 0, (PASSWORD_SIZE - size) * 4);
	for (uint32_t i = 0; i < size; i++)
		password[i] = x[i + 1];
}

Password::Password(const string &x) :
	size(x.length())
{
	memset(password + size, 0, (PASSWORD_SIZE - size) * 4);
	for (uint32_t i = 0; i < size; i++)
		password[size - (i + 1)] = find(alpha_list, alpha_list_end, (uint8_t)x[i]) - alpha_list;
}

void Password::operator =(uint64_t x)
{
	memset(password, 0, 256);
	size = 1;
	
	while (x)
	{
		password[size - 1] = x % alpha_list_size;
		x /= alpha_list_size;
		
		if (x)
		{
			size++;
			x--;
		}
	}
}

void Password::operator =(const Password &x)
{
	size = x.size;
	memcpy(password, x.password, 256);
}

bool Password::operator ==(const Password &x) const
{
	if (size != x.size)
		return false;
	
	for (uint32_t i = 0; i < size; i++)
		if (password[i] != x.password[i])
			return false;
	
	return true;
}

bool Password::operator !=(const Password &x) const
{
	return !this->operator ==(x);
}

bool Password::operator <(const Password &x) const
{
	if (size != x.size)
		return size < x.size;
	
	for (uint32_t i = 0; i < size; i++)
		if (password[i] != x.password[i])
			return password[i] < x.password[i];
	
	return false;
}

bool Password::operator >(const Password &x) const
{
	if (size != x.size)
		return size > x.size;
	
	for (uint32_t i = 0; i < size; i++)
		if (password[i] != x.password[i])
			return password[i] > x.password[i];
	
	return false;
}

bool Password::operator <=(const Password &x) const
{
	if (size != x.size)
		return size < x.size;
	
	for (uint32_t i = 0; i < size; i++)
		if (password[i] != x.password[i])
			return password[i] < x.password[i];
	
	return true;
}

bool Password::operator >=(const Password &x) const
{
	if (size != x.size)
		return size > x.size;
	
	for (uint32_t i = 0; i < size; i++)
		if (password[i] != x.password[i])
			return password[i] > x.password[i];
	
	return true;
}

void Password::operator +=(uint64_t x)
{
	uint32_t i = 1;
	
	while (x)
	{
		x += password[i - 1];
		password[i - 1] = x % alpha_list_size;
		x /= alpha_list_size;
		
		if (x)
			if (++i > size)
				x--;
	}
	
	if (i > size)
		size = i;
}

/*void Password::operator +=( const Password &x )
 {
 
 size = getmax( size, x.size );
 uint32_t i = 0;
 const uint32_t sizem1 = size - 1;
 
 for ( ; i < sizem1; i++ )
 {
 
 password[i] += x.password[i];
 
 if ( password[i] >= alpha_list_size )
 {
 password[i] -= alpha_list_size;
 password[i + 1]++;
 }
 
 }
 
 password[i] += x.password[i];
 
 if ( password[i] >= alpha_list_size )
 {
 password[i] -= alpha_list_size;
 size++;
 }
 
 }*/

void Password::operator ++(int32_t)
{
	for (uint32_t i = 0; i < size; i++)
	{
		password[i]++;
		
		if (password[i] == alpha_list_size)
			password[i] = 0;
		else
			return;
	}
	size++;
}

bool Password::Add(uint64_t x)
{
	uint32_t i = 1;
	
	while (x)
	{
		
		x += password[i - 1];
		password[i - 1] = x % alpha_list_size;
		x /= alpha_list_size;
		
		if (x)
			if (++i > size)
			{
				if (i == PASSWORD_SIZE)
					return false;
				else
					x--;
			}
		
	}
	
	if (i > size)
		size = i;
	
	return true;
}

/*bool Password::Add( const Password &x )
 {
 
 size = getmax( size, x.size );
 uint32_t i = 0;
 const uint32_t sizem1 = size - 1;
 
 for ( ; i < sizem1; i++ )
 {
 
 password[i] += x.password[i];
 
 if ( password[i] >= alpha_list_size )
 {
 password[i] -= alpha_list_size;
 password[i + 1]++;
 }
 
 }
 
 password[i] += x.password[i];
 
 if ( password[i] >= alpha_list_size )
 {
 
 if ( size == PASSWORD_SIZE )
 return false;
 
 password[i] -= alpha_list_size;
 
 size++;
 
 }
 
 return true;
 
 }*/

uint64_t Password::MaxAdd(void) const
{
	static const uint32_t alpha_list_sizem1 = alpha_list_size - 1;
	uint64_t res = 0;
	
	for (uint32_t i = size; i > 0; --i)
	{
		res += alpha_list_sizem1 - password[i];
		res *= alpha_list_sizem1;
	}
	
	return res;
}

uint64_t Password::Sub(const Password &x) const
{
	int32_t rem = 0;
	int32_t sum;
	
	Password tmp;
	
	tmp.size = size;
	
	for (uint32_t i = 0; i < size; i++)
	{
		sum = password[i] - x.password[i] - rem;
		
		//cout << "+\t" << password[i] << endl;
		//cout << "-\t" << x.password[i] << endl;
		//cout << "=\t" << sum << endl;
		
		if (i != 0 && i < x.size)
			sum--;
		
		//cout << "=\t" << sum << endl;
		
		if (sum < 0)
		{
			sum += alpha_list_size;
			rem = 1;
		}
		else
		{
			rem = 0;
		}
		//cout << "=\t" << sum << endl << endl;
		
		tmp.password[i] = sum;
	}
	
	if (rem != 0)
	{
		tmp.size--;
		//cout << "rem" << endl;
	}
	
	return tmp.Convert();
}

uint64_t Password::Convert(void) const
{
	uint64_t res = password[0] + 0;
	
	for (uint32_t i = 1; i < size; i++)
		res += (password[i] + 1) * pow(alpha_list_size, i);
	
	return res;
}

const uint32_t *Password::Get1(void) const
{
	for (uint32_t i = 0; i < size; i++)
		pwd1[i] = alpha_list[password[i]];
	
	return pwd1;
}

/*const uint8_t *Password::Get2( void ) const
 {
 
 pwd2[0] = size;
 
 for ( uint32_t i = 0; i < size; i++ )
 pwd2[i+1] = alpha_list[password[i]];
 
 return pwd2;
 
 }*/

const uint32_t Password::Size(void) const
{
	return size;
}

const int32_t Password::Print(ostream &os) const
{
	for (uint32_t i = 0; i < size; i++)
		os.put(alpha_list[password[size - (i + 1)]]);
	
	return 0;
}

string Password::String(void) const
{
	string res;
	
	res.append(1, size);
	
	for (uint32_t i = 0; i < size; i++)
		res.append(1, password[i]);
	
	return res;
}

ostream &operator <<(ostream &os, const Password &x)
{
	os.put(x.size);
	for (uint32_t i = 0; i < x.size; i++)
		os.put(x.password[i]);
	
	return os;
}

istream &operator >>(istream &is, Password &x)
{
	x.size = (uint8_t)is.get();
	
	memset(x.password + x.size, 0, (PASSWORD_SIZE - x.size) * 4);
	//memset( password, 0, 256 );
	
	for (uint32_t i = 0; i < x.size; i++)
		x.password[i] = (uint8_t)is.get();
	
	return is;
}

