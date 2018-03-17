/*
 * WorkUnit.h
 *
 *  Created on: 17. mar. 2018
 *      Author: Peter S. Balling
 */

#ifndef WORKUNIT_H_
#define WORKUNIT_H_

#include "Password.h"
#include "HashData.h"

#include <vector>
using namespace std;

#include <pthread.h>

struct SubWorkUnit
{
	SubWorkUnit() :
		progress(0),
		completed(0),
		chunk_size(0),
		private_id(0),
		private_progress(NULL),
		private_end(NULL)
	{
	}
	
	pthread_mutex_t sub_mutex;
	
	uint64_t progress;
	uint64_t completed;
	
	uint64_t chunk_size;
	
	vector<struct Coordinates> collision;
	
	int32_t private_id;
	uint64_t *private_progress;
	uint64_t *private_end;
};

struct WorkUnit
{
	WorkUnit() :
		safe_to_save(false),
		terminate(false),
		id(0),
		x_size(0),
		y_size(0),
		data_size(0),
		data(NULL),
		data_end(NULL)
	{
	}
	
	pthread_cond_t save_cond;
	pthread_mutex_t save_mutex;
	bool safe_to_save;
	
	bool terminate;
	
	uint32_t id;
	
	Password x_pwd;
	uint64_t x_size;
	
	Password y_pwd;
	uint64_t y_size;
	
	uint64_t data_size;
	struct HashData *data;
	struct HashData *data_end;
	
	struct SubWorkUnit swu;
};

#endif /* WORKUNIT_H_ */
