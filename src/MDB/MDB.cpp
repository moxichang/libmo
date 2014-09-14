/**********************************************************
 * Filename:    MDB.cpp
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

#include "Exception.h" 

#include "MDB/MDB.h"
#include "MDB/MDBDriver_Oracle.h"
#include "MDB/MDBDriver_Mysql.h"

const int DriverLength = 2;

char Drivers[][10] = 
	{
		"oracle",
		"mysql"
	};

MDB::MDBDriver* MDB::MDBFactory:: Factory(std::string dsn_)
{

	std::string type;
	std::string username;
	std::string password;
	std::string host;
	std::string database;
	int port;

	MDBDriver* driver = NULL;

	ParseDSN(dsn_, type, username, password, host, port, database);

	if(DriverAvailable(type) == false)
	{
		Throw("Driver specified not impelmented", MException::ME_INVARG);
	}

	if(type.compare("oracle") == 0)
	{
		driver = new MDBDriver_Oracle(username, password, database);	
	}
	else if( type.compare("mysql") == 0)
	{
		// FIXME mysql driver not impelment
		driver = new MDBDriver_Mysql(username, password, host, database);	
	}
	else
	{
		Throw("MDB Driver specified not exised.",MException::ME_INVARG);
	}

	return driver;
}

MDB::MDBDriver* MDB::MDBFactory:: Singleton(std::string dsn_)
{

	return NULL;
}

void MDB::MDBFactory:: ParseDSN(std::string dsn,
		std::string& type_, std::string& username_, std::string& password_,
		std::string& host_, int& port_, std::string& database_)
{

	if(dsn.empty())
	{
		Throw("DSN string can not be empty", MException::ME_INVARG);
	}

	// find database type, this is essential
	size_t pos = dsn.find("://");

	if(pos == std::string::npos || pos == 0)
	{
		Throw("Database type not found", MException::ME_INVARG);
	}
	else
	{
		type_ = dsn.substr(0, pos);
	}

	if(dsn.erase(0, pos + 3).empty())  // erase the database type filed
	{
		Throw("Username and password field not found", MException::ME_INVARG);
	}

	// find username and password
	// username is essential
	// password is not (use empty as default value)
	pos = dsn.find_first_of('@');

	if(pos == std::string::npos || pos == 0)
	{
		Throw("Username and password field not found", MException::ME_INVARG);
	}

	std::string user_info = dsn.substr(0, pos);

	if(dsn.erase(0, pos+1).empty())
	{
		Throw("Host name not found", MException::ME_INVARG);
	}

	pos = user_info.find_first_of(':');
	if(pos == 0) // the username not exised
	{
		Throw("Username field not found", MException::ME_INVARG);
	}
	else if(pos == std::string::npos)
	{
		username_ = user_info.substr(0, pos);
		password_ = "";
	}
	else 
	{
		username_ = user_info.substr(0, pos);
		password_ = user_info.substr(pos+1, std::string::npos);
	}


	// find hostname and port
	pos = dsn.find_first_of('/');

	if(pos == std::string::npos || pos == 0)
	{
		Throw("Host name not found", MException::ME_INVARG);
	}

	std::string host_info = dsn.substr(0, pos);

	if(dsn.erase(0, pos+1).empty())
	{
		Throw("Database name not found", MException::ME_INVARG);
	}

	pos = host_info.find_first_of(':');
	if(pos == 0)
	{
		Throw("Hostname not found", MException::ME_INVARG);
	}
	else if(pos == std::string::npos)
	{
		host_ = host_info.substr(0, pos);
		port_ = -1;
	}
	else
	{

		host_ = host_info.substr(0, pos);
		if((port_ = atoi(host_info.substr(pos+1, std::string::npos).c_str())) ==0 )
		{
			Throw("Port must be a positive integer", MException::ME_INVARG);
		}
	}

	// find database name
	database_= dsn;

#ifdef MDB_DEBUG
	cout<<"type:\t\t"<<type_<<endl;
	cout<<"username:\t"<<username_<<endl;
	cout<<"password:\t"<<password_<<endl;
	cout<<"host:\t\t"<<host_<<endl;
	cout<<"database:\t"<<database_<<endl;
	cout<<"port:\t\t"<<port_<<endl;
#endif

}

bool MDB::MDBFactory:: DriverAvailable(std::string driver_name_)
{
	for(int i=0; i< DriverLength; i++)
	{
		if(driver_name_.compare(Drivers[i]) == 0)
		{
			return true;
		}
	}

	return false;
}

