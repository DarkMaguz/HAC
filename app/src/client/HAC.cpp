/*
 * HAC.cpp
 *
 *  Created on: 16/07/2009
 *      Author: Peter S. Balling
 */

#include "HAC.h"

void *Run(void *arg)
{
	struct WorkUnit *wu = (struct WorkUnit *)arg;
	vector<struct Coordinates> collision;
	
	pthread_mutex_lock(&wu->swu.sub_mutex);
	
	const int32_t id = wu->swu.private_id;
	wu->swu.private_id++;
	
	Password pwd(wu->x_pwd);
	uint64_t progress = wu->swu.private_progress[id];
	uint64_t end = wu->swu.private_end[id];
	
	pthread_mutex_unlock(&wu->swu.sub_mutex);
	
	if (progress != 0)
		pwd += progress;
	
	while (1)
	{
		while (progress < end && !wu->terminate)
		{
			struct HashData x(pwd);
			
			if (binary_search(wu->data, wu->data_end, x))
			{
				struct HashData *y = find(wu->data, wu->data_end, x);
				while (y != wu->data_end && y->hash == x.hash)
				{
					Password y_pwd(wu->y_pwd);
					y_pwd += y->id;
					
					if (y_pwd != pwd)
						collision.push_back(Coordinates(progress, y->id));
					
					y++;
				}
			}
			
			pwd++;
			progress++;
		}
		
		pthread_mutex_lock(&wu->swu.sub_mutex);
		
		if (wu->swu.progress == wu->x_size || wu->terminate)
		{
			wu->swu.private_progress[id] = progress;
			wu->swu.private_end[id] = end;
			
			if (!collision.empty())
				wu->swu.collision.insert(wu->swu.collision.end(), collision.begin(), collision.end());
			
			pthread_mutex_unlock(&wu->swu.sub_mutex);
			return 0;
		}
		
		pwd = wu->x_pwd;
		progress = wu->swu.progress;
		end = progress + wu->swu.chunk_size;
		
		pwd += progress;
		
		if (end > wu->x_size)
			end = wu->x_size;
		
		wu->swu.progress = end;
		
		pthread_mutex_unlock(&wu->swu.sub_mutex);
	}
	
	return 0;
}

void *AutoSave(void *arg)
{
	HAC *client = (HAC *)arg;
	int32_t res;
	struct timespec ts;
	struct timeval tv;
	
	while (!client->workUnit.terminate)
	{
		pthread_mutex_lock(&client->workUnit.save_mutex);
		
		gettimeofday(&tv, NULL);
		TIMEVAL_TO_TIMESPEC(&tv, &ts);
		ts.tv_sec += AUTOSAVE_TIME;
		res = pthread_cond_timedwait(&client->workUnit.save_cond, &client->workUnit.save_mutex, &ts);
		
		pthread_mutex_unlock(&client->workUnit.save_mutex);
		
		if (res == ETIMEDOUT)
		{
			pthread_mutex_lock(&client->workUnit.swu.sub_mutex);
			if (client->workUnit.safe_to_save)
				client->Save();
			pthread_mutex_unlock(&client->workUnit.swu.sub_mutex);
		}
	}
	
	return 0;
}

void *SignalHandler(void *arg)
{
	HAC *client = (HAC *)arg;
	int32_t res;
	
	while (!client->workUnit.terminate)
	{
		sigwait(&client->signalMask, &res);
		switch (res)
		{
			case SIGINT:
			case SIGTERM:
			{
				client->Exit();
				break;
			}
			default:
				break;
		}
	}
	
	return 0;
}

HAC::HAC() :
		exitType(0),
		clientSocket(SERVER_HOST_NAME, SERVER_PORT)
{
	pthread_t sig_thr_id;
	
	sigemptyset(&signalMask);
	sigaddset(&signalMask, SIGINT);
	sigaddset(&signalMask, SIGTERM);
	
	pthread_sigmask( SIG_BLOCK, &signalMask, NULL);
	
	pthread_create(&sig_thr_id, NULL, SignalHandler, this);
	
	LoadConfig();
	
	if (benchmark == 0)
	{
		cout << "Loading benchmark test..." << endl;
		benchmark = BenchMark() / core_count;
		SaveConfig();
	}
}

HAC::~HAC()
{
}

void HAC::Start(void)
{
	sysinfo(&sysInfo);
	
	workUnit.data_size = ram_percentage * 1000; //( info.freeram / 100 * ram_percentage ) / HASHDATA_SIZE;
	workUnit.data = new struct HashData[workUnit.data_size];
	
	workUnit.swu.private_progress = new uint64_t[cpu_max];
	workUnit.swu.private_end = new uint64_t[cpu_max];
	
	exitType = 1;
	
	pthread_mutex_init(&workUnit.swu.sub_mutex, NULL);
	pthread_mutex_init(&workUnit.save_mutex, NULL);
	pthread_cond_init(&workUnit.save_cond, NULL);
	
	pthread_t thread_as;
	pthread_t threadid[cpu_max];
	uint64_t i;
	
	ClearWU();
	
	Load();
	
	if (!Auth())
		Exit();
	
	if (workUnit.y_size > workUnit.data_size)
		Free();
	else
		Send();
	
	if (workUnit.id == 0)
	{
		NetStream ns(clientSocket);
		Get();
		ns << 5;
		clientSocket.Close();
		
		Save();
	}
	else
	{
		NetStream ns(clientSocket);
		ns << 5;
		clientSocket.Close();
	}
	
	PrepareWU();
	
	exitType = 2;
	workUnit.safe_to_save = true;
	
	pthread_create(&thread_as, NULL, AutoSave, this);
	
	while (!workUnit.terminate)
	{
		cout << "Starting HAC: \t";
		for (i = 0; i < core_count; i++)
			pthread_create(&threadid[i], NULL, Run, &workUnit);
		cout << "[SUCCESS]" << endl;
		
		for (i = 0; i < core_count; i++)
			pthread_join(threadid[i], NULL);
		
		pthread_mutex_lock(&workUnit.swu.sub_mutex);
		workUnit.safe_to_save = false;
		pthread_mutex_unlock(&workUnit.swu.sub_mutex);
		
		pthread_mutex_lock(&workUnit.save_mutex);
		pthread_cond_signal(&workUnit.save_cond);
		pthread_mutex_unlock(&workUnit.save_mutex);
		
		if (!workUnit.terminate)
		{
			pthread_mutex_lock(&workUnit.swu.sub_mutex);
			
			workUnit.swu.completed = workUnit.swu.progress;
			
			if (!Auth())
				break;
			
			NetStream ns(clientSocket);
			
			Send();
			if (workUnit.terminate)
				break;
			
			ClearWU();
			
			Get();
			
			if (workUnit.terminate)
				break;
			
			ns << 5;
			
			clientSocket.Close();
			
			Save();
			
			pthread_mutex_unlock(&workUnit.swu.sub_mutex);
			
			PrepareWU();
			
			workUnit.safe_to_save = true;
		}
	}
	Save();
	
	pthread_cond_destroy(&workUnit.save_cond);
	pthread_mutex_destroy(&workUnit.save_mutex);
	pthread_mutex_destroy(&workUnit.swu.sub_mutex);
	delete[] workUnit.swu.private_progress;
	delete[] workUnit.swu.private_end;
	delete[] workUnit.data;
}

uint64_t HAC::BenchMark(void)
{
	sysinfo(&sysInfo);
	
	workUnit.data_size = (sysInfo.freeram / 100 * ram_percentage) / HASHDATA_SIZE;
	workUnit.data = new struct HashData[workUnit.data_size];
	
	workUnit.swu.private_progress = new uint64_t[cpu_max];
	workUnit.swu.private_end = new uint64_t[cpu_max];
	
	exitType = 1;
	
	ClearWU();
	
	pthread_mutex_init(&workUnit.swu.sub_mutex, NULL);
	
	uint64_t score = 0, i;
	
	pthread_t threadid[core_count];
	
	workUnit.y_size = getmin( sysInfo.freeram, 1024 * 1024 * 5 ) / HASHDATA_SIZE;
	workUnit.data_end = workUnit.data + workUnit.y_size;
	
	workUnit.x_pwd += workUnit.y_size * 2;
	workUnit.x_size = 1024 * 1024 * 1724;
	
	PrepareWU();
	
	cout << "Running benchmark: \t";
	cout.flush();
	
	uint64_t backup_chunk_size = workUnit.swu.chunk_size;
	workUnit.swu.chunk_size = workUnit.y_size / core_count;
	
	for (i = 0; i < core_count; i++)
		pthread_create(&threadid[i], NULL, Run, &workUnit);
	
	sleep( BENCHMARK_TIME);
	workUnit.terminate = true;
	
	for (i = 0; i < core_count; i++)
		pthread_join(threadid[i], NULL);
	
	score = workUnit.swu.progress;
	
	for (i = 0; i < core_count; i++)
		score -= workUnit.swu.private_end[i] - workUnit.swu.private_progress[i];
	
	score /= BENCHMARK_TIME;
	
	workUnit.swu.chunk_size = backup_chunk_size;
	
	pthread_mutex_destroy(&workUnit.swu.sub_mutex);
	delete[] workUnit.swu.private_progress;
	delete[] workUnit.swu.private_end;
	delete[] workUnit.data;
	
	cout << "[SUCCESS]" << endl;
	
	exitType = 0;
	
	return score;
}

enum wu_args
{
	wu_x_password = 0,
	wu_x_size,
	wu_x_completed,
	wu_y_password,
	wu_y_size,
	wu_0
};

void HAC::Debug(const string path)
{
	exitType = 2;
	
	map<string, wu_args> wu_map;
	wu_map["x_password"] = wu_x_password;
	wu_map["x_size"] = wu_x_size;
	wu_map["x_completed"] = wu_x_completed;
	wu_map["y_password"] = wu_y_password;
	wu_map["y_size"] = wu_y_size;
	
	fstream fs(path.c_str());
	if (fs.is_open())
	{
		workUnit.swu.private_progress = new uint64_t[cpu_max];
		workUnit.swu.private_end = new uint64_t[cpu_max];
		
		ClearWU();
		workUnit.swu.collision.clear();
		
		string wu_line;
		uint64_t found;
		
		while (!fs.eof())
		{
			wu_line.clear();
			
			getline(fs, wu_line);
			
			wu_line.erase(remove_if(wu_line.begin(), wu_line.end(), is_space), wu_line.end());
			
			found = wu_line.find('#');
			if (found != string::npos)
				wu_line.erase(found);
			
			if (!wu_line.empty())
			{
				found = wu_line.find('=');
				switch (wu_map[wu_line.substr(0, found)])
				{
					case wu_x_password:
					{
						workUnit.x_pwd = wu_line.substr(found + 1);
						break;
					}
					case wu_x_size:
					{
						workUnit.x_size = atoll(wu_line.substr(found + 1).c_str());
						break;
					}
					case wu_x_completed:
					{
						workUnit.swu.completed = atoll(wu_line.substr(found + 1).c_str());
						break;
					}
					case wu_y_password:
					{
						workUnit.y_pwd = wu_line.substr(found + 1);
						break;
					}
					case wu_y_size:
					{
						workUnit.y_size = atoll(wu_line.substr(found + 1).c_str());
						break;
					}
					default:
						break;
				}
			}
		}
		fs.close();
		
		workUnit.data_size = workUnit.y_size;
		workUnit.data = new struct HashData[workUnit.data_size];
		workUnit.data_end = workUnit.data + workUnit.y_size;
		
		pthread_mutex_init(&workUnit.swu.sub_mutex, NULL);
		
		PrepareWU();
		
		pthread_t threadid[cpu_max];
		
		cout << "Starting HAC: \t";
		
		uint64_t i;
		workUnit.swu.collision.clear();
		for (i = 0; i < core_count; i++)
			pthread_create(&threadid[i], NULL, Run, &workUnit);
		
		cout << "[SUCCESS]" << endl;
		
		for (i = 0; i < core_count; i++)
			pthread_join(threadid[i], NULL);
		
		cout << endl;
		cout << "wu.x_pwd = ";
		workUnit.x_pwd.Print(cout);
		cout << endl;
		cout << "wu.x_size = " << workUnit.x_size << endl;
		cout << "wu.swu.completed = " << workUnit.swu.completed << endl;
		cout << "wu.y_pwd = ";
		workUnit.y_pwd.Print(cout);
		cout << endl;
		cout << "wu.y_size = " << workUnit.y_size << endl;
		cout << endl;
		
		cout << "wu.swu.collision.size() = " << workUnit.swu.collision.size() << endl;
		if (!workUnit.swu.collision.empty())
		{
			uint64_t fcount1 = 0;
			uint64_t fcount2 = 0;
			uint64_t fcount3 = 0;
			uint64_t fcount4 = 0;
			
			for (i = 0; i < workUnit.swu.collision.size(); i++)
			{
				Password tmp_xpwd(workUnit.x_pwd);
				tmp_xpwd += workUnit.swu.collision[i].x;
				
				Password tmp_ypwd(workUnit.y_pwd);
				tmp_ypwd += workUnit.swu.collision[i].y;
				
				Hash tmp_hash1(tmp_xpwd);
				Hash tmp_hash2(tmp_ypwd);
				
				if (tmp_xpwd != tmp_ypwd)
				{
					fcount1++;
					if (tmp_hash1 == tmp_hash2)
					{
						fcount3++;
					}
				}
				else
				{
					fcount2++;
					if (tmp_hash1 == tmp_hash2)
					{
						fcount4++;
					}
				}
			}
			
			cout << fcount1 << endl;
			cout << fcount3 << endl;
			cout << fcount2 << endl;
			cout << fcount4 << endl;
		}
		
		pthread_mutex_destroy(&workUnit.swu.sub_mutex);
		delete[] workUnit.swu.private_progress;
		delete[] workUnit.swu.private_end;
		delete[] workUnit.data;
	}
	else
	{
		cerr << "[FAILED]" << endl;
		cerr << "Error opening \"" << path << "\"." << endl;
	}
}

void HAC::Exit(int32_t status)
{
	cout << endl << "Shutting down." << endl;
	
	switch (exitType)
	{
		case 0:
		{
			exit(status);
			break;
		}
		case 1:
		{
			delete[] workUnit.swu.private_progress;
			delete[] workUnit.swu.private_end;
			delete[] workUnit.data;
			
			exit(status);
			break;
		}
		case 2:
		{
			workUnit.terminate = true;
			break;
		}
		default:
			break;
	}
}

void HAC::ClearWU(void)
{
	workUnit.terminate = false;
	workUnit.id = 0;
	workUnit.x_size = 0;
	workUnit.y_size = 0;
	workUnit.swu.progress = 0;
	workUnit.swu.completed = 0;
	workUnit.swu.private_id = 0;
	
	memset(workUnit.swu.private_progress, 0, cpu_max * 8);
	memset(workUnit.swu.private_end, 0, cpu_max * 8);
}

void *Build(void *arg)
{
	struct WorkUnit *wu = (struct WorkUnit *)arg;
	
	pthread_mutex_lock(&wu->swu.sub_mutex);
	
	Password pwd(wu->y_pwd);
	
	uint64_t progress = wu->swu.progress;
	wu->swu.progress += wu->swu.chunk_size;
	uint64_t end = wu->swu.progress;
	
	pthread_mutex_unlock(&wu->swu.sub_mutex);
	
	pwd += progress;
	while (progress < end)
	{
		wu->data[progress].id = progress;
		wu->data[progress].hash = pwd;
		
		pwd++;
		progress++;
	}
	
	return 0;
}

void HAC::PrepareWU(void)
{
	uint64_t i;
	pthread_t threadid[core_count];
	
	uint64_t backup_progress = workUnit.swu.progress;
	workUnit.swu.progress = 0;
	
	workUnit.swu.chunk_size = workUnit.y_size / core_count;
	
	cout << "Building matrix: \t";
	cout.flush();
	
	for (i = 0; i < core_count; i++)
		pthread_create(&threadid[i], NULL, Build, &workUnit);
	
	for (i = 0; i < core_count; i++)
		pthread_join(threadid[i], NULL);
	
	if (workUnit.swu.progress < workUnit.y_size)
	{
		Password pwd(workUnit.y_pwd);
		pwd += workUnit.swu.progress;
		
		while (workUnit.swu.progress < workUnit.y_size)
		{
			workUnit.data[workUnit.swu.progress].id = workUnit.swu.progress;
			workUnit.data[workUnit.swu.progress].hash = pwd;
			
			pwd++;
			workUnit.swu.progress++;
		}
	}
	
	sort(workUnit.data, workUnit.data_end);
	
	cout << "[SUCCESS]" << endl;
	
	workUnit.swu.progress = backup_progress;
	workUnit.swu.chunk_size = (workUnit.x_size - workUnit.swu.progress) / core_count;
}

void HAC::Save(void)
{
	cout << "Saving..." << endl;
	cout << "File: \"" << WORKUNIT_PATH << "\"\t";
	cout.flush();
	
	if (file_exists( WORKUNIT_PATH))
		rename( WORKUNIT_PATH, WORKUNIT_PATH_BAK);
	
	fstream fs( WORKUNIT_PATH, ios::out | ios::trunc | ios::binary);
	if (fs.is_open())
	{
		fs.write((char *)&workUnit.id, 4);
		
		fs << workUnit.x_pwd;
		fs.write((char *)&workUnit.x_size, 8);
		
		fs << workUnit.y_pwd;
		fs.write((char *)&workUnit.y_size, 8);
		
		fs.write((char *)&workUnit.swu.progress, 8);
		
		fs.write((char *)&core_count, 8);
		
		fs.write((char *)workUnit.swu.private_progress, 8 * core_count);
		fs.write((char *)workUnit.swu.private_end, 8 * core_count);
		
		uint64_t collisions = workUnit.swu.collision.size();
		fs.write((char *)&collisions, 8);
		for (vector<struct Coordinates>::iterator it = workUnit.swu.collision.begin(); it < workUnit.swu.collision.end(); it++)
			fs.write((char *)&(*it), 16);
		
		fs.close();
		
		cout << "[SUCCESS]" << endl;
		
		if (file_exists( WORKUNIT_PATH_BAK))
			remove( WORKUNIT_PATH_BAK);
	}
	else
	{
		if (file_exists( WORKUNIT_PATH_BAK))
		{
			if (file_exists( WORKUNIT_PATH))
				remove( WORKUNIT_PATH);
			rename( WORKUNIT_PATH_BAK, WORKUNIT_PATH);
		}
		
		cerr << "[FAILED]" << endl;
		cerr << "Error writing \"" << WORKUNIT_PATH << "\"." << endl;
	}
}

void HAC::Load(void)
{
	cout << "File: \"" << WORKUNIT_PATH << "\"\t";
	cout.flush();
	
	if (file_exists( WORKUNIT_PATH))
	{
		fstream fs( WORKUNIT_PATH, ios::in | ios::binary);
		if (fs.is_open())
		{
			fs.read((char *)&workUnit.id, 4);
			
			fs >> workUnit.x_pwd;
			fs.read((char *)&workUnit.x_size, 8);
			
			fs >> workUnit.y_pwd;
			fs.read((char *)&workUnit.y_size, 8);
			
			fs.read((char *)&workUnit.swu.progress, 8);
			uint64_t private_size;
			fs.read((char *)&private_size, 8);
			
			if (private_size > cpu_max)
			{
				delete[] workUnit.swu.private_progress;
				delete[] workUnit.swu.private_end;
				
				workUnit.swu.private_progress = new uint64_t[private_size];
				workUnit.swu.private_end = new uint64_t[private_size];
			}
			
			fs.read((char *)workUnit.swu.private_progress, 8 * private_size);
			fs.read((char *)workUnit.swu.private_end, 8 * private_size);
			
			uint64_t collisions;
			fs.read((char *)&collisions, 8);
			
			struct Coordinates tmp_collision;
			for (uint64_t i = 0; i < collisions; i++)
			{
				fs.read((char *)&tmp_collision, 16);
				workUnit.swu.collision.push_back(tmp_collision);
			}
			
			fs.close();
			
			workUnit.swu.completed = workUnit.swu.private_progress[0];
			for (uint64_t i = 1; i < private_size; i++)
				if (workUnit.swu.completed > workUnit.swu.private_progress[i])
					workUnit.swu.completed = workUnit.swu.private_progress[i];
			
			if (workUnit.y_size > workUnit.data_size)
			{
				cerr << "[FAILED]" << endl;
				cerr << "The size of this work unit exceeds the available memory." << endl;
			}
			else
			{
				while (private_size < core_count)
				{
					workUnit.swu.private_progress[private_size] = 0;
					workUnit.swu.private_end[private_size] = 0;
					private_size++;
				}
				
				sort(workUnit.swu.private_progress, workUnit.swu.private_progress + private_size);
				sort(workUnit.swu.private_end, workUnit.swu.private_end + private_size);
				
				if (private_size > core_count)
				{
					workUnit.swu.progress = workUnit.swu.private_progress[core_count];
					
					// Just to make things clear! They won't be used any further... or ???
					memset(workUnit.swu.private_progress + core_count, 0, private_size - core_count);
					memset(workUnit.swu.private_end + core_count, 0, private_size - core_count);
				}
				
				workUnit.data_end = workUnit.data + workUnit.y_size;
				cout << "[SUCCESS]" << endl;
			}
		}
		else
		{
			cerr << "[FAILED]" << endl;
			cerr << "Error opening \"" << WORKUNIT_PATH << "\"." << endl;
			Exit();
		}
	}
	else
	{
		cout << "Not found." << endl;
	}
}

bool HAC::Connect(void)
{
	cout << "Establishing connection to server: ";
	cout.flush();
	
	clientSocket = ClientSocket( SERVER_HOST_NAME, SERVER_PORT);
	if (!clientSocket.Connect(3))
	{
		cerr << "[FAILED]" << endl;
		cerr << "Unable to connect whit server: " << SERVER_HOST_NAME << ":" << SERVER_PORT << endl;
		return false;
	}
	
	NetStream ns(clientSocket);
	
	cout << "[SUCCESS]" << endl;
	
	ns << user_id;
	
	if (user_id == 0)
		ns.Write(user.c_str(), user.length());
	
	return true;
}

bool HAC::Auth(void)
{
	
	if (!Connect())
		return false;
	
	cout << "Authenticating user: ";
	cout.flush();
	
	NetStream ns(clientSocket);
	ns << 3;
	
	bool auth_ok;
	ns >> auth_ok;
	
	if (auth_ok)
	{
		if (user_id == 0)
			ns >> user_id;
		cout << "[SUCCESS]" << endl;
	}
	else
	{
		cerr << "[FAILED]" << endl;
		
		ns >> user_id;
		
		clientSocket.Close();
		
		if (user_id == 0)
			cerr << "Unknown user." << endl;
		else
			cerr << "This account has not yet been activated." << endl;
	}
	
	return auth_ok;
}

void HAC::Send(void)
{
	if (!workUnit.swu.collision.empty())
		SendCollision();
	
	if (workUnit.swu.completed == 0 || workUnit.id == 0)
		return;
	
	cout << "Sending completed work units: ";
	cout.flush();
	
	NetStream ns(clientSocket);
	ns << 1;
	
	ns << workUnit.id;
	ns << workUnit.swu.completed;
	
	bool status;
	ns >> status;
	
	cout << "[SUCCESS]" << endl;
	
	if (!status)
		ClearWU();
}

void HAC::SendCollision(void)
{
	cout << "Sending data: ";
	cout.flush();
	
	NetStream ns(clientSocket);
	
	ns << 4;
	
	ns << workUnit.id;
	
	ns << workUnit.swu.collision;
	
	workUnit.swu.collision.clear();
	
	cout << "[SUCCESS]" << endl;
}

void HAC::Get(void)
{
	int32_t retry = 0;
	int32_t timeout = 5;
	bool status = false;
	
	while (retry < 5 && !status)
	{
		if (retry != 0)
		{
			cerr << "Unable to get work unit from server." << endl;
			cerr << "Retrying in " << timeout << " seconds. (" << retry << "/" << 5 << ")" << endl;
			
			sleep(timeout);
			
			if (Auth())
				Exit();
		}
		
		cout << "Requesting new work unit from server: ";
		cout.flush();
		
		NetStream ns(clientSocket);
		ns << 2;
		
		ns << workUnit.data_size;
		ns << benchmark * core_count;
		
		ns >> status;
		
		if (status)
		{
			ns >> workUnit.x_pwd;
			ns >> workUnit.x_size;
			ns >> workUnit.swu.progress;
			ns >> workUnit.y_pwd;
			ns >> workUnit.y_size;
			ns >> workUnit.id;
			
			workUnit.data_end = workUnit.data + workUnit.y_size;
			workUnit.swu.completed = workUnit.swu.progress;
			
			cout << "[SUCCESS]" << endl;
		}
		else
		{
			retry++;
			timeout *= retry;
			
			cerr << "[FAILED]" << endl;
			
			ns << 5;
			clientSocket.Close();
		}
	}
	
	if (!status)
	{
		cerr << "Failed to retrieve any work units from server." << endl;
		Exit();
	}
}

void HAC::Free(void)
{
	if (!workUnit.swu.collision.empty())
		SendCollision();
	
	if (workUnit.id == 0)
		return;
	
	NetStream ns(clientSocket);
	
	ns << 3;
	
	ns << workUnit.id;
	ns << workUnit.swu.completed;
	
	bool status;
	ns >> status;
	
	ClearWU();
}

void HAC::Activate(void)
{
	cout << "Please type in the activation code that has been sent to you by email." << endl;
	
	bool activated = false;
	while (!activated)
	{
		
		string activation_code;
		cout << "Activation code: ";
		getline(cin, activation_code);
		
		if (!Connect())
			return;
		
		NetStream ns(clientSocket);
		
		cout << "Sending activation code to server: ";
		cout.flush();
		
		ns << 2;
		ns.Write(activation_code.c_str(), activation_code.length());
		
		ns >> activated;
		
		if (user_id == 0)
			ns >> user_id;
		
		clientSocket.Close();
		
		if (!activated)
		{
			cerr << "[FAILED]" << endl;
			
			if (user_id == 0)
			{
				cerr << "Unknown user." << endl;
				//user.clear();
				//SaveConfig();
				return;
			}
			cerr << "Invalid activation code." << endl;
		}
	}
	
	cout << "[SUCCESS]" << endl;
	cout << "Your account is now activated." << endl;
}

void HAC::Register(string new_email)
{
	int32_t res = 0;
	const static string goodchars = "-_.";
	
	cout << "Registration..." << endl;
	
	// Check user
	uint64_t userlen = user.length();
	if (userlen < 6)
	{
		cout << "Invalid user: To short, it must be at least 6." << endl;
		return;
	}
	else
	{
		for (uint64_t i = 0; i < userlen; i++)
			if (!isalnum(user[i]))
			{
				if (i == 0 || i == userlen - 1)
				{
					cout << "Invalid user: First and last sign must be alphanumeric." << endl;
					return;
				}
				else if (goodchars.find(user[i]) == string::npos)
				{
					cout << "Invalid user: Illegal sign \"" << user[i] << "\"." << endl;
					return;
				}
			}
	}
	
	uint64_t esign_pos = new_email.find('@');
	// Check email
	if (spc_email_isvalid(new_email.c_str()))
	{
		if (gethostbyname(new_email.substr(esign_pos + 1).c_str()) == NULL)
		{
			cout << "Invalid email: Unable to contact email server." << endl;
			return;
		}
	}
	else
	{
		cout << "Invalid email: Email contains Illegal sign." << endl;
		return;
	}
	
	// Send registration
	if (!Connect())
		return;
	
	NetStream ns(clientSocket);
	if (!ns.IsGood())
		cout << "fail fail fail.............." << endl;
	
	cout << "Sending registration data to server: ";
	cout.flush();
	
	ns << 1;
	ns.Write(new_email.c_str(), new_email.length());
	ns >> res;
	
	clientSocket.Close();
	
	switch (res)
	{
		case 0:
		{
			cout << "[SUCCESS]" << endl;
			cout << "The account is now registered." << endl;
			cout << "An email containing an activation code will be delivered to your email shortly." << endl;
			SaveConfig();
			break;
		}
		case 7:
		{
			cerr << "[FAILED]" << endl;
			cerr << "A user with that name already exist." << endl;
			break;
		}
		case 8:
		{
			cerr << "[FAILED]" << endl;
			cerr << "A user with that email already exist." << endl;
			break;
		}
		default:
		{
			cerr << "[FAILED]" << endl;
			cerr << "Unknown error." << endl;
			break;
		}
	}
}

