/*
 * Utils.cpp
 *
 *  Created on: 18-11-2008
 *      Author: Peter S. Balling
 */

#include "Utils.h"

/* return true if successful */
bool mkfile(const char *path, bool overwrite)
{
	if (is_file(path) && !overwrite)
		return false;
	
	fstream fs(path, ios::out | ios::trunc | ios::binary);
	if (!fs.is_open())
		return false;
	
	fs.close();
	return true;
}

#ifdef __linux__
bool makedir(const char *path, bool overwrite)
{
	if (is_dir(path))
	{
		if (overwrite)
			removedir(path);
		else
			return false;
	}
	
	mkdir(path, S_IRWXU);
	return true;
}

bool removedir(string path)
{
	if (path[path.length() - 1] != DIR_CHAR)
		path += DIR_CHAR;
	
	DIR *d = opendir(path.c_str());
	if (d == NULL)
	{
		cout << "error opendir: " << path << endl;
		return false;
	}
	
	struct dirent *de = NULL;
	while ((de = readdir(d)) != NULL)
	{
		if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
		{
			string c_path = path;
			c_path.append(de->d_name);
			
			if (is_dir(c_path.c_str()))
				removedir(c_path);
			else
				remove(c_path.c_str());
		}
	}
	closedir(d);
	
	if (rmdir(path.c_str()) != 0)
	{
		cout << "error rmdir: " << path.c_str() << endl;
		return false;
	}
	
	return true;
}

bool file_exists(const char *path)
{
	struct stat x;
	stat(path, &x);

	return S_ISREG( x.st_mode ) || S_ISDIR(x.st_mode);
}

/* return true if it is a file*/
bool is_file(const char *path)
{
	struct stat x;
	stat(path, &x);
	
	return S_ISREG(x.st_mode);
}

/* return true if it is a directory*/
bool is_dir(const char *path)
{
	struct stat x;
	stat(path, &x);
	
	return S_ISDIR(x.st_mode);
}

bool get_dir_content(const char *path, vector<string> &list)
{
	DIR *d = opendir(path);
	if (d != NULL)
	{
		struct dirent *de;
		while ((de = readdir(d)) != NULL)
			list.push_back(de->d_name);
	}
	else
		return false;
	
	closedir(d);
	return true;
}

const char *get_cwd(void)
{
	return getcwd( NULL, 0);
}

unsigned long long get_file_size(const char *path)
{
	struct stat x;
	stat(path, &x);
	
	return x.st_size;
}
#endif

bool copyfile(const char *src, const char *dst)
{
	if (!file_exists(src))
		return false;
	
	if (!mkfile(dst))
		return false;
	
	fstream src_fs(src, ios::in | ios::binary);
	
	src_fs.seekg(0, ios::end);
	unsigned long long size = src_fs.tellg();
	src_fs.seekg(0, ios::beg);
	
	char *buffer = new char[size];
	
	src_fs.read(buffer, size);
	src_fs.close();
	
	fstream dst_fs(dst, ios::out | ios::binary);
	dst_fs.write(buffer, size);
	dst_fs.close();
	
	delete[] buffer;
	
	return true;
}

/* http://www.oreillynet.com/pub/a/network/excerpt/spcookbook_chap03/index3.html */ // thx m8
int spc_email_isvalid(const char *address)
{
	int count = 0;
	const char *c, *domain;
	static const char *rfc822_specials = "()<>@,;:\\\"[]";
	
	/* first we validate the name portion (name@domain) */
	for (c = address; *c; c++)
	{
		if (*c == '\"' && (c == address || *(c - 1) == '.' || *(c - 1) == '\"'))
		{
			while (*++c)
			{
				if (*c == '\"')
					break;
				if (*c == '\\' && (*++c == ' '))
					continue;
				if (*c <= ' ' || *c >= 127)
					return 0;
			}
			
			if (!*c++)
				return 0;
			if (*c == '@')
				break;
			if (*c != '.')
				return 0;
			continue;
		}
		
		if (*c == '@')
			break;
		if (*c <= ' ' || *c >= 127)
			return 0;
		if (strchr(rfc822_specials, *c))
			return 0;
	}
	
	if (c == address || *(c - 1) == '.')
		return 0;
	
	/* next we validate the domain portion (name@domain) */
	if (!*(domain = ++c))
		return 0;
	
	do
	{
		if (*c == '.')
		{
			if (c == domain || *(c - 1) == '.')
				return 0;
			count++;
		}
		
		if (*c <= ' ' || *c >= 127)
			return 0;
		if (strchr(rfc822_specials, *c))
			return 0;
	} while (*++c);
	
	return (count >= 1);
}

string str_date(void)
{
	time_t t = time( NULL);
	tm *now = localtime(&t);
	char *buffer = new char[19];
	
	sprintf(buffer, "%d-%.2d-%.2d %.2d:%.2d:%.2d", now->tm_year + 1900, now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	string res(buffer, 19);
	
	delete[] buffer;
	
	return res;
}
