/*
 * NetStream.h
 *
 *  Created on: 24/07/2009
 *      Author: Peter S. Balling
 */

#ifndef NETSTREAM_H_
#define NETSTREAM_H_

#include "Socket.h"

#include <unistd.h>

#include <vector>
#include <iterator>
#include <iostream>
using namespace std;

class NetStream: public Socket
{
	public:
		NetStream(const Socket &sockfd);
		virtual ~NetStream();
		
		template<typename T>
		NetStream &operator <<(const T &data)
		{
			Write(reinterpret_cast<const char *>(&data), sizeof(T));
			return *this;
		}
		
		template<typename T>
		NetStream &operator >>(T &data)
		{
			Read((char *)&data);
			return *this;
		}
		
		template<typename T>
		NetStream &operator <<(const vector<T> &data)
		{
			Write(reinterpret_cast<const char *>(data.data()), sizeof(T) * data.size());
			return *this;
		}
		
		template<typename T>
		NetStream &operator >>(vector<T> &data)
		{
			int32_t len;
			
			if (!ReadSize(reinterpret_cast<char *>(&len)))
				return *this;
			
			data.resize(len / sizeof(T));
			Reader(reinterpret_cast<char *>(data.data()), len);
			
			return *this;
		}
		
		NetStream &operator <<(string &data)
		{
			Write(reinterpret_cast<const char *>(data.c_str()), data.size());
			return *this;
		}
		
		NetStream &operator >>(string &data)
		{
			int32_t len;
			
			if (!ReadSize(reinterpret_cast<char *>(&len)))
				return *this;
			
			char *tmp = new char[len];
			
			Reader(tmp, len);
			data.assign(tmp, len);

			delete[] tmp;
			
			return *this;
		}
		
		bool Write(const char *data, const int32_t &len);
		bool Read(char *data, int32_t &len);
		bool Read(char *data, uint32_t &ulen);
		bool Read(char *data);
		
	private:
		bool WriteSize(const char *size);
		bool ReadSize(char *size);
		
		bool Reader(char *data, const int32_t &len);
};

#endif /* NETSTREAM_H_ */
