/**********************************************************
 * Filename:    MDBDriver.h
 * Text Encoding: utf-8
 *
 * Description:
 *              
 *
 * Author: moxichang (ishego@gmail.com)
 *         Harbin Engineering University 
 *         Information Security Research Center
 *
 *********************************************************/

#include "MDB/MDBDriver.h"

MDB::MDBDriver:: MDBDriver(std::string username_, std::string password_,
		std::string database_, std::string host_, int port_)
	: username_m(username_), password_m(password_),
	database_m(database_), host_m(host_), port_m(port_)
{

}

MDB::MDBDriver:: ~MDBDriver()
{

}

std::string GetDSN()
{
	return "dns";
}
