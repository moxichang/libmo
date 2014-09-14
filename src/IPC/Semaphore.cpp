/**********************************************************
 * Filename:    Semaphore.cpp
 * Text Encoding: utf-8
 *
 * Description:
 *              
 *
 * Author: moxichang (ishego@gmail.com)
 *         Harbin Engineering University 
 *         Information Security Research Center
 *
 * Create Data:	2008-08-10
 * Last Update: 2008-12-18
 *
 *********************************************************/

#include <sstream>
#include <stdexcept>

#include <semaphore.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "Exception.h"
#include "IPC/Semaphore.h"

#define PERMS (mode_t)(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FLAGS (O_CREAT | O_EXCL)
#define INITVALUE 1

Semaphore::Semaphore(const int val_)
	: name_m(""), sem_m(NULL)
{
	ref_counter = new int(1);

	this->GenName();
	this->GetNamed(this->name_m.c_str(), &sem_m, val_);
}

Semaphore::Semaphore(const std::string name_,int val_)
	: name_m(name_), sem_m(NULL)
{
	ref_counter = new int(1);

	this->GetNamed(name_.c_str(), &sem_m, val_);
}

Semaphore::Semaphore(const Semaphore& sem_)
	: name_m(sem_.name_m),
	sem_m(sem_.sem_m)
{
	ref_counter = sem_.ref_counter;
	*(ref_counter)++;
}

Semaphore::~Semaphore()
{
	if(!--(*ref_counter)){
		sem_close(sem_m);
		delete ref_counter;
	}

	// FIXME: do we need to delete the path in destructor ?
	// 因为其他进程中也有可能存在指向当前路径的信号量
	// 这种情况下，删除会不会导致问题？
	//sem_unlink(this->name.c_str());
}

Semaphore& Semaphore::operator = (Semaphore& sem_)
{
	name_m = sem_.name_m;
	sem_m = sem_.sem_m;

	ref_counter = sem_.ref_counter;
	*(ref_counter)++;

	return sem_;
}

int Semaphore::GenName()
{
	std::stringstream base;
	base<<"/tmp.semaphore."<<getpid()<<"."<<rand();
	this->name_m = base.str();

	return 0;
}

int Semaphore::GetNamed(const char *name, sem_t **sem, int val) {

	int err;

	// try to create a named semaphore
        while (((*sem = sem_open(name, FLAGS , PERMS, val)) == SEM_FAILED) &&
                        ( (err = errno) == EINTR)) ;

        if (*sem != SEM_FAILED)
                return 0;

        if (err != EEXIST){
		std::stringstream msg;
		msg<<"create named semaphore with path:'"<<name<<"' failed";
		Throw(msg.str(), err);
	}

	// named semaphore already exised, open it
        while (((*sem = sem_open(name, 0)) == SEM_FAILED) && ((err = errno) == EINTR)) ;

        if (*sem != SEM_FAILED)
                return 0;
	else{
		std::stringstream msg;
		msg<<"create named semaphore with path:'"<<name<<"' failed";
		Throw(msg.str(), err);
	}

        return -1;
}

void Semaphore::Wait(){

	int retval;
	int err;

	while(((retval = sem_wait(this->sem_m)) == -1) && ((err = errno) == EINTR));

	if(retval == -1){
		std::stringstream msg;
		msg<<"wait on named semaphore "<<name_m<<" failed";
		Throw(msg.str(), err);
	}
}

void Semaphore::Post(){

	int retval;
	int err;

	while(((retval = sem_post(this->sem_m)) == -1) && ((err = errno) == EINTR));
	
	if(retval == -1){
		std::stringstream msg;
		msg<<"post on named semaphore "<<name_m<<" failed";
		Throw(msg.str(), err);
	}
}
