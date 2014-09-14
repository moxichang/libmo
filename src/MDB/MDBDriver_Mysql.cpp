/**********************************************************
 * Filename:    MDBDriver_Mysql.cpp
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
#include "Log.h"

#include "MDB/MDBDriver_Mysql.h"
#include "MDB/MDBResult_Mysql.h"

MDB::MDBDriver_Mysql:: MDBDriver_Mysql(std::string username_,std::string password_, std::string host_,std::string database_, int port_)
	: MDBDriver(username_, password_, database_, host_, port_)
{
	InitEnviorment();
	Connect();
}

MDB::MDBDriver_Mysql:: ~MDBDriver_Mysql()
{
	Disconnect();
	mysql_server_end();
}

void MDB::MDBDriver_Mysql:: InitEnviorment()
{

	if(mysql_server_init(0, NULL, NULL))
	{
		Throw(mysql_error(mysql), MException::ME_RUNTIME);
	}

	if((mysql = mysql_init(NULL)) == NULL)
	{
		Throw(mysql_error(mysql), MException::ME_RUNTIME);
	}

	mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "libmysqld_client");

	mysql_options(mysql, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL);

}

void MDB::MDBDriver_Mysql:: Connect()
{
	if(mysql_real_connect(mysql, host_m.c_str(), username_m.c_str(), password_m.c_str(), database_m.c_str(), port_m, NULL, 0) == NULL)
	{
		Throw(mysql_error(mysql), MException::ME_RUNTIME);
	}
}

void MDB::MDBDriver_Mysql:: Disconnect()
{
	mysql_close(mysql);
}

MDB::MDBResult* MDB::MDBDriver_Mysql:: Query(std::string sql_)
{

	MDBResult *result = NULL;

	if(mysql_query(mysql, sql_.c_str()))
	{
		Throw(mysql_error(mysql), MException::ME_RUNTIME);
	}

	MYSQL_RES *res;
	if( (res = mysql_store_result(mysql)) == NULL)
	{
		Throw(mysql_error(mysql), MException::ME_RUNTIME);
	}

	try{
		result = new MDBResult_Mysql(mysql, res);
	}
	catch(std::exception &e)
	{
		// 如果在创建MDBResult_Mysql类对象的构造函数中发生异常
		// 将不会调用MDBResult_Mysql的析构函数，因此在这里释放资源
		mysql_free_result(res);

		Throw(e.what(),MException::ME_RETHROW);
	}

	return result;
}

MDB::MDBResult* MDB::MDBDriver_Mysql:: Query(const MDB::MDBStatement* stm_)
{

	return NULL;
}

unsigned long long MDB::MDBDriver_Mysql:: Execute(std::string sql_)
{
	unsigned long affected;

	if(mysql_query(mysql, sql_.c_str()))
	{
		Throw(mysql_error(mysql), MException::ME_RUNTIME);
	}

	// FIXME
	// mysql_affected_rows返回无符号数值
	// 手册上说和(my_ulonglong)-1比较即可，但这么做是否理解正确？
	// http://dev.mysql.com/doc/refman/5.1/zh/apis.html#mysql-affected-rows
	if((affected = mysql_affected_rows(mysql)) == (my_ulonglong)-1)
	{
		Throw(mysql_error(mysql), MException::ME_RUNTIME);
	}

	return affected;
}

unsigned long long MDB::MDBDriver_Mysql:: Execute(const MDBStatement* stm_)
{
	return 0;
}

MDB::MDBStatement* MDB::MDBDriver_Mysql:: Prepare(std::string sql_)
{
	return NULL;
}

void MDB::MDBDriver_Mysql:: Commit()
{

}

void MDB::MDBDriver_Mysql:: Rollback()
{

}


