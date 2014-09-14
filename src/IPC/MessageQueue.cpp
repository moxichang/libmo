/**********************************************************
 * Filename:    MessageQueue.cpp
 * Text Encoding: utf-8
 *
 * Description:
 *              
 *
 * Author: moxichang (ishego@gmail.com)
 *         Harbin Engineering University 
 *         Information Security Research Center
 *
 * Create Data:	2008-12-18
 * Last Update: 2008-12-18
 *
 *********************************************************/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <string>
#include <sstream>

#include "Exception.h"
#include "IPC/MessageQueue.h"

MessageQueue::MessageQueue(const std::string path_)
	: path_m(path_),
	key_m(0),
	msq_id_m(0),
	msg_type_m(0)
{
	ref_counter = new int(1);

	Create(path_);
}

/*
MessageQueue::MessageQueue(key_t key_)
	: path_m(""),
	key_m(key_),
	msq_id_m(0),
	msg_type_m(0)

{
	ref_counter = new int(1);

	msq_id_m = msgget(key_, IPC_CREAT|IPC_EXCL | 00666);
}
*/

MessageQueue::MessageQueue(const int msq_id_)
	: path_m(""),
	key_m(0),
	msq_id_m(msq_id_),
	msg_type_m(0)

{
	ref_counter = new int(1);
}

MessageQueue::MessageQueue(MessageQueue& msq_)
	: path_m(msq_.path_m),
	key_m(msq_.key_m),
	msq_id_m(msq_.msq_id_m),
	msg_type_m(0)
{
	ref_counter = msq_.ref_counter;

	*(ref_counter)++;
}

MessageQueue::~MessageQueue()
{
	if(!--(*ref_counter)){
		delete ref_counter;
		this->Destory();
		//FIXME delete the message queue here?
	}
}

MessageQueue& MessageQueue::operator = (MessageQueue& msq_)
{
	path_m = msq_.path_m;
	key_m = msq_.key_m;
	msq_id_m = msq_.msq_id_m;
	msg_type_m = 0;

	ref_counter = msq_.ref_counter;
	*(ref_counter)++;

	return msq_;
}

void MessageQueue::Create(const std::string path_)
{
	path_m = path_;

	if(path_.empty()){
		key_m = IPC_PRIVATE;
	}
	else{
		key_m = ftok(path_.c_str(),'a');
	}

	//FIXME !!! what fuck is 00666?
	//http://www.ibm.com/developerworks/cn/linux/l-ipc/part3/
	if((msq_id_m = msgget(key_m, IPC_CREAT | IPC_EXCL | 00666)) == -1){
		std::stringstream msg;
		msg<<"create message queue with path '"<<path_<<"'";
		Throw(msg.str(), errno);
	}
}

void MessageQueue::Destory()
{
	if(msgctl(msq_id_m, IPC_RMID, NULL) == -1){
		Throw("remove message queue failed",errno);
	}
}

void MessageQueue::Send(const struct MessagePacket* packet_)
{
	if(msgsnd(msq_id_m, packet_, sizeof(packet_->data), IPC_NOWAIT) == -1){
		Throw("send data through message queue failed", errno);
	}
}

void MessageQueue::Recv(struct MessagePacket* packet_)
{
	if(msgrcv(msq_id_m, packet_, sizeof(packet_->data), msg_type_m, MSG_NOERROR) == -1){
		Throw("read data on message queue failed", errno);
	}
}
