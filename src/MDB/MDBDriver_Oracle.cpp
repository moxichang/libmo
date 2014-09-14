/**********************************************************
 * Filename:    MDBDriver_Oracle.cpp
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

#include <signal.h>

#include "Exception.h"
#include "Log.h"

#include "MDB/MDBDriver_Oracle.h"
#include "MDB/MDBResult_Oracle.h"
#include "MDB/MDBStatement_Oracle.h"

#include <string>

MDB::MDBDriver_Oracle:: MDBDriver_Oracle(std::string username_, std::string password_, std::string database_) 
	: MDBDriver(username_, password_, database_, "")
{

	envhp = (OCIEnv *)0;
	srvhp = (OCIServer *)0;
	svchp = (OCISvcCtx *)0;
	errhp = (OCIError *)0;
	authp = (OCISession *)0;

	InitEnviorment();

	Connect();

#ifdef MDB_DEBUG
	LOG(LOG_DEBUG, "Connect to database and login success.");
#endif

}


MDB::MDBDriver_Oracle:: ~MDBDriver_Oracle()
{
	Disconnect();
}

void MDB::MDBDriver_Oracle:: InitEnviorment()
{
	struct sigaction old_act;

	CheckError( OCIInitialize ( OCI_THREADED | OCI_OBJECT, (dvoid *)0,
			(dvoid * (*)(dvoid *, size_t)) 0,
			(dvoid * (*)(dvoid *, dvoid *, size_t))0,
			(void (*)(dvoid *, dvoid *)) 0 ) );

	// 初始化环境
	CheckError( OCIEnvInit ((OCIEnv **) &envhp, OCI_DEFAULT, (size_t) 0,
			(dvoid **) 0) );

	// 创建错误描述句柄
	CheckError( OCIHandleAlloc ((dvoid *) envhp, (dvoid **) &errhp, OCI_HTYPE_ERROR,
			(size_t) 0, (dvoid **) 0) );

	// 创建服务器上下文句柄
	CheckError( OCIHandleAlloc ((dvoid *) envhp, (dvoid **) &srvhp, OCI_HTYPE_SERVER,
			(size_t) 0, (dvoid **) 0) );

	CheckError( OCIHandleAlloc ((dvoid *) envhp, (dvoid **) &svchp, OCI_HTYPE_SVCCTX,
			(size_t) 0, (dvoid **) 0) );

	try
	{
		// OCI在OCIServerAttach后会屏蔽 SIGINT 信号，有可能影响用户程序行为
		// 为了处理这个问题，在OCIServerAttach调用之前先保存旧的信号处理句柄（有可能是SIG_DFL，也有可能是用户自定义的）
		// 在OCIServerAttach调用完成之后恢复
		if(sigaction(SIGINT, NULL, &old_act) == -1)
		{
			Throw("get original signal handler for SIGINT failed", errno);
		}

		// 连接到数据库
		sword status = OCIServerAttach (srvhp, errhp, (text *)database_m.c_str(),
					database_m.length(), 0);

		// 回复原先的信号处理句柄
		if(sigaction(SIGINT, &old_act, NULL) == -1)
		{
			Throw("restore signal handler for SIGINT failed", errno);
		}

		CheckError(status);
	}
	catch(std::exception &e)
	{
		if (srvhp) OCIHandleFree((dvoid *) srvhp, (CONST ub4) OCI_HTYPE_SERVER);
		if (svchp) OCIHandleFree((dvoid *) svchp, (CONST ub4) OCI_HTYPE_SVCCTX);
		if (errhp) OCIHandleFree((dvoid *) errhp, (CONST ub4) OCI_HTYPE_ERROR);

		// xian add 2011-9-19 15:52:51
		if (envhp) OCIHandleFree((dvoid *) envhp, (CONST ub4) OCI_HTYPE_ENV);
		
		Throw(e.what(), MException::ME_RETHROW);
	}

	// FIXME 错误检查？
	// set attribute server context in the service context
	CheckError( OCIAttrSet ((dvoid *) svchp, OCI_HTYPE_SVCCTX, (dvoid *)srvhp,
			(ub4) 0, OCI_ATTR_SERVER, (OCIError *) errhp) );

}

void MDB::MDBDriver_Oracle:: Connect()
{

	// 分配会话句柄
	CheckError( OCIHandleAlloc((dvoid *) envhp, (dvoid **)&authp,
			(ub4) OCI_HTYPE_SESSION, (size_t) 0, (dvoid **) 0) );

	// Attention: username
	// 设置会话句柄属性（用户名）
	CheckError( OCIAttrSet((dvoid *) authp, (ub4) OCI_HTYPE_SESSION,
			(dvoid *) username_m.c_str(), (ub4)username_m.length(),
			(ub4) OCI_ATTR_USERNAME, errhp) );
	// Attention: password
	// 设置会话句柄属性（密码）
	CheckError( OCIAttrSet((dvoid *) authp, (ub4) OCI_HTYPE_SESSION,
			(dvoid *) password_m.c_str(), (ub4) password_m.length(),
			(ub4) OCI_ATTR_PASSWORD, errhp) );

	// 开始会话
	CheckError(OCISessionBegin ( svchp,  errhp, authp, OCI_CRED_RDBMS,
				(ub4) OCI_DEFAULT));

	// 将会话句柄放入服务上下文句柄中
	CheckError( OCIAttrSet((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX,
			(dvoid *) authp, (ub4) 0,
			(ub4) OCI_ATTR_SESSION, errhp) );
}

void MDB::MDBDriver_Oracle:: Disconnect()
{
	OCISessionEnd (svchp, errhp, authp, (ub4) 0);

	OCIServerDetach (srvhp, errhp, (ub4) OCI_DEFAULT );

	if (authp) OCIHandleFree((dvoid *) authp, (CONST ub4) OCI_HTYPE_SESSION);
	if (svchp) OCIHandleFree((dvoid *) svchp, (CONST ub4) OCI_HTYPE_SVCCTX);
	if (srvhp) OCIHandleFree((dvoid *) srvhp, (CONST ub4) OCI_HTYPE_SERVER);
	if (errhp) OCIHandleFree((dvoid *) errhp, (CONST ub4) OCI_HTYPE_ERROR);

	if (envhp) OCIHandleFree((dvoid *) envhp, (CONST ub4) OCI_HTYPE_ENV);
}

MDB::MDBResult* MDB::MDBDriver_Oracle:: Query(std::string sql_)
{

	OCIStmt *stmthp = NULL;
	sword status = 0;
	MDBResult* result = NULL;

#ifdef MDB_DEBUG
	LOG(LOG_DEBUG, "Execute query: %s",sql_.c_str());
#endif

	try{

		// 这个句柄将在下面创建的MDBResult_Oracle对象中被释放
		CheckError(OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &stmthp,
					OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0));

		CheckError(OCIStmtPrepare(stmthp, errhp, (text *)sql_.c_str(),
					(ub4) sql_.length(),
					(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));

		if ((status = OCIStmtExecute(svchp, stmthp, errhp, (ub4) 0, (ub4) 0,
						(CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_STMT_SCROLLABLE_READONLY)))
		{
			if (stmthp)
			{
				OCIHandleFree((dvoid *) stmthp, (CONST ub4) OCI_HTYPE_STMT);
			}

			if(status == OCI_NO_DATA){
#ifdef MDB_DEBUG
				LOG(LOG_DEBUG,"OCI_NO_DATA returned.");
#endif
				result = NULL;
			}
			else
			{
				CheckError(status);
				result = NULL;
			}
		}
		else{

#ifdef MDB_DEBUG
			LOG(LOG_DEBUG, "Fetch columns info");
#endif
			ub2 numcols;
			MDBColumn** columns = FetchColumns(stmthp, numcols);

			// FIXME 这个实现比较的恶心，有没有更好地方法？
			// 为MDBResult_Oracle对象分配错误句柄
			// 这个句柄将在MDBResult_Oracle中被释放
			OCIError *result_errhp;
			(void) OCIHandleAlloc ((dvoid *) envhp, (dvoid **) &result_errhp, OCI_HTYPE_ERROR,
					(size_t) 0, (dvoid **) 0);

			result = new MDBResult_Oracle(stmthp, result_errhp, columns, numcols);
		}

	}
	catch(std::exception& e)
	{
		// 如果发生异常，释放statement句柄，重新抛出异常
		if (stmthp)
		{
			OCIHandleFree((dvoid *) stmthp, (CONST ub4) OCI_HTYPE_STMT);
		}

		Throw(e.what(), MException::ME_RETHROW);
	}

#ifdef MDB_DEBUG
	LOG(LOG_DEBUG, "Query done.");
#endif

	return result;
}

MDB::MDBResult* MDB::MDBDriver_Oracle:: Query(const MDBStatement* stm_)
{
	
	OCIStmt *stmthp = NULL;
	sword status = 0;
	MDBResult* result = NULL;

	// FIXME
	// 很不好看，想想有什么其他方法
	stmthp = (OCIStmt *)(stm_->GetStatmentHandler());

	if ((status = OCIStmtExecute(svchp, stmthp, errhp, (ub4) 0, (ub4) 0,
				(CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_STMT_SCROLLABLE_READONLY)))
	{
		if(status == OCI_NO_DATA){
#ifdef MDB_DEBUG
			LOG(LOG_DEBUG,"OCI_NO_DATA returned.");
#endif
		}
		else
		{
			CheckError(status);
		}
		
		result = NULL;
	}
	else{

#ifdef MDB_DEBUG
		LOG(LOG_DEBUG, "Fetch columns info");
#endif
		ub2 numcols;
		MDBColumn** columns = FetchColumns(stmthp, numcols);

		// 为MDBResult_Oracle对象分配错误句柄
		OCIError *result_errhp;
		(void) OCIHandleAlloc ((dvoid *) envhp, (dvoid **) &result_errhp, OCI_HTYPE_ERROR,
				(size_t) 0, (dvoid **) 0);

		result = new MDBResult_Oracle(stmthp, result_errhp, columns, numcols);
	}

#ifdef MDB_DEBUG
	LOG(LOG_DEBUG, "Query done.");
#endif

	return result;
}


unsigned long long MDB::MDBDriver_Oracle:: Execute(std::string sql_)
{

	OCIStmt *stmthp = NULL;
	sword status = 0;
	unsigned long long affected = 0;

#ifdef MDB_DEBUG
	LOG(LOG_DEBUG, "Execute query: %s",sql_.c_str());
#endif

	try{

		CheckError(OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &stmthp,
					OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0));

		CheckError(OCIStmtPrepare(stmthp, errhp, (text *)sql_.c_str(),
					(ub4) sql_.length(),
					(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));

		if ((status = OCIStmtExecute(svchp, stmthp, errhp, (ub4) 1, (ub4) 0,
					(CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT))
					&& status != OCI_SUCCESS_WITH_INFO )
		{
			if (stmthp)
			{
				OCIHandleFree((dvoid *) stmthp, (CONST ub4) OCI_HTYPE_STMT);
			}

			CheckError(status);
	
			return affected;
		}
		else if(status == OCI_SUCCESS_WITH_INFO){
#ifdef MDB_DEBUG
			LOG(LOG_DEBUG,"Query execute success, but have warnings.");
#endif
		}

		CheckError(OCIAttrGet ((dvoid *)stmthp, (ub4)OCI_HTYPE_STMT,
					(dvoid *)&affected, (ub4 *) 0, (ub4)OCI_ATTR_ROW_COUNT, errhp));

	}
	catch(std::exception& e)
	{
		// 如果发生异常，释放statement句柄，重新抛出异常
		if (stmthp)
		{
			OCIHandleFree((dvoid *) stmthp, (CONST ub4) OCI_HTYPE_STMT);
		}	

		Throw(e.what(), MException::ME_RETHROW);
	}

#ifdef MDB_DEBUG
	LOG(LOG_DEBUG, "Query done.");
#endif

	// 释放 statement 句柄
	if (stmthp)
	{
		OCIHandleFree((dvoid *) stmthp, (CONST ub4) OCI_HTYPE_STMT);
	}

	return affected;
}

unsigned long long MDB::MDBDriver_Oracle:: Execute(const MDBStatement* stm_)
{
	OCIStmt *stmthp = NULL;
	sword status = 0;
	long affected = 0;

	// FIXME
	// 很不好看，想想有什么其他方法
	stmthp = (OCIStmt *)(stm_->GetStatmentHandler());

	if ((status = OCIStmtExecute(svchp, stmthp, errhp, (ub4) 1, (ub4) 0,
			(CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT))
			&& status != OCI_SUCCESS_WITH_INFO )
	{
		CheckError(status);
	}
	else if(status == OCI_SUCCESS_WITH_INFO){
#ifdef MDB_DEBUG
		LOG(LOG_DEBUG,"Query execute success, but have warnings.");
#endif
	}

	CheckError(OCIAttrGet ((dvoid *)stmthp, (ub4)OCI_HTYPE_STMT,
				(dvoid *)&affected, (ub4 *) 0, (ub4)OCI_ATTR_ROW_COUNT, errhp));

#ifdef MDB_DEBUG
	LOG(LOG_DEBUG, "Query done.");
#endif

	return affected;

}

MDB::MDBStatement* MDB::MDBDriver_Oracle:: Prepare(std::string sql_)
{
	OCIStmt *stmthp = NULL;
	OCIError *stm_errhp = NULL;

	try{

		// 为即将创建的MDBStatement对象分配句柄
		// 这个句柄将在MDBStatement中被释放
		CheckError(OCIHandleAlloc ((dvoid *) envhp, (dvoid **) &stm_errhp, OCI_HTYPE_ERROR,
					(size_t) 0, (dvoid **) 0));

		CheckError(OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &stmthp,
					OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0));

		CheckError(OCIStmtPrepare(stmthp, errhp, (text *)sql_.c_str(),
					(ub4) sql_.length(),
					(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));

	}
	catch(std::exception& e)
	{
		if(stmthp)
		{
			OCIHandleFree((dvoid *) stmthp, (CONST ub4) OCI_HTYPE_STMT);	
		}

		if(stm_errhp)
		{
			OCIHandleFree((dvoid *) stm_errhp, (CONST ub4) OCI_HTYPE_ERROR);
		}

		throw e;
	}

	return (new MDBStatement_Oracle(stmthp, stm_errhp));
}

void MDB::MDBDriver_Oracle:: Commit()
{
	CheckError(OCITransCommit(svchp, errhp, OCI_DEFAULT));
}

void MDB::MDBDriver_Oracle:: Rollback()
{
	CheckError(OCITransRollback(svchp, errhp, OCI_DEFAULT));
}

MDB::MDBColumn** MDB::MDBDriver_Oracle:: FetchColumns(OCIStmt *stmthp_, ub2& numcols_)
{

	OCIParam *parmp;

	ub2 dtype;
	ub2  precision;                //包括小数点的总位数
	sb1  scale = 0;    

	text *col_name;
	ub4 col_name_len;

	ub4 counter;
	ub2 numcols;

	ub4 char_semantics;
	ub2 col_width;

	MDBColumn **columns = NULL;

	numcols = 0;
	CheckError( OCIAttrGet ((dvoid *)stmthp_, (ub4)OCI_HTYPE_STMT, 
				(dvoid *)&numcols, (ub4 *) 0, (ub4)OCI_ATTR_PARAM_COUNT, errhp));

#ifdef MDB_DEBUG
	LOG(LOG_DEBUG, "Col number: %d", numcols);
#endif

	// for output
	numcols_ = numcols;

	if(numcols != 0 ){
		columns = new MDBColumn*[numcols];
	}
	else{
		//FIXME 
		// error handling needed here or is it necessary? 
		// because the checkerror function will throw exception before this statement
	}

	for(counter = 1; counter <= numcols; counter++)
	{
		// Request a parameter descriptor for position 'counter' in the select-list
		// 获取参数描述符
		CheckError(OCIParamGet((dvoid *)stmthp_, OCI_HTYPE_STMT, errhp,
					(dvoid **)&parmp, (ub4) counter));

		// Retrieve the datatype attribute 
		// 获取列数据类型
		CheckError(OCIAttrGet((dvoid*) parmp, (ub4) OCI_DTYPE_PARAM,
					(dvoid*) &dtype,(ub4 *) 0, (ub4) OCI_ATTR_DATA_TYPE,
					(OCIError *) errhp  ));

		// Retrieve the column name attribute
		// 获取列名和列名长度
		col_name_len = 0;
		CheckError(OCIAttrGet((dvoid*) parmp, (ub4) OCI_DTYPE_PARAM,
					(dvoid**) &col_name, (ub4 *) &col_name_len, (ub4) OCI_ATTR_NAME,
					(OCIError *) errhp ));

		CheckError(OCIAttrGet((dvoid*) parmp, (ub4) OCI_DTYPE_PARAM,
					(dvoid*) &precision,(ub4 *) 0, (ub4) OCI_ATTR_PRECISION,
					(OCIError *) errhp  ));

		CheckError(OCIAttrGet((dvoid*) parmp, (ub4) OCI_DTYPE_PARAM,
					(dvoid*) &scale,(ub4 *) 0, (ub4) OCI_ATTR_SCALE,
					(OCIError *) errhp  ));


		// Retrieve the length semantics for the column 
		// FIXME What the fuck dose 'length semantics' mean?
		char_semantics = 0;
		CheckError(OCIAttrGet((dvoid*) parmp, (ub4) OCI_DTYPE_PARAM,
					(dvoid*) &char_semantics,(ub4 *) 0, (ub4) OCI_ATTR_CHAR_USED,
					(OCIError *) errhp  ));
		col_width = 0;
		if (char_semantics){
			// Retrieve the column width in characters
			CheckError(OCIAttrGet((dvoid*) parmp, (ub4) OCI_DTYPE_PARAM,
						(dvoid*) &col_width, (ub4 *) 0, (ub4) OCI_ATTR_CHAR_SIZE,
						(OCIError *) errhp  ));
		}
		else {
			// Retrieve the column width in bytes
			CheckError(OCIAttrGet((dvoid*) parmp, (ub4) OCI_DTYPE_PARAM,
						(dvoid*) &col_width,(ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,
						(OCIError *) errhp  ));
		}

#ifdef MDB_DEBUG
		LOG(LOG_DEBUG, "%d     %s     %d", dtype, col_name, col_width);
		LOG(LOG_DEBUG, "%d %d %d", dtype, precision, scale)
#endif
		if(dtype == SQLT_NUM && (scale == 0 || scale == -127))
		{
			dtype = SQLT_INT;
		}
		
		columns[counter-1] = new MDBColumn( TransColumnType(dtype), (char *)col_name, col_width);

	}

	return columns;
}

MDB::ColumnType MDB::MDBDriver_Oracle:: TransColumnType(ub2 dtype)
{

	ColumnType dt;

	switch(dtype)
	{
		case SQLT_CHR:
			dt = VARCHAR;
			break;
		case SQLT_NUM:
			dt = FLOAT;
			break;
		case SQLT_INT:
			dt = INTEGER;
			break;
		case SQLT_FLT:
			dt = FLOAT;
			break;
		case SQLT_DAT:
			dt = DATE;
			break;
		default:
			dt = UNKNOWN;
			Throw("unsupport column type.", MException::ME_INVARG);
	}

	return dt;
}

void MDB::MDBDriver_Oracle:: CheckError(sword status_)
{
	/*! FIXME
	 * @bug 512字节的errbuf是否足够？
	 */
	text errbuf[512];
	sb4 errcode = 0;

	switch (status_)
	{
		case OCI_SUCCESS:
			break;
		case OCI_SUCCESS_WITH_INFO:
#ifdef DEBUG_MDB
			(void) printf("Error - OCI_SUCCESS_WITH_INFO\n");
#endif
			break;
		case OCI_NEED_DATA:
#ifdef DEBUG_MDB
			(void) printf("Error - OCI_NEED_DATA\n");
#endif

			Throw("OCI_NEED_DATA", MException::ME_RUNTIME);

			break;
		case OCI_NO_DATA:
#ifdef DEBUG_MDB
			(void) printf("Error - OCI_NODATA\n");
#endif

			Throw("OCI_NODATA", MException::ME_RUNTIME);

			break;
		case OCI_ERROR:
			(void) OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode,
					errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
#ifdef DEBUG_MDB
			(void) printf("Error - %.*s\n", 512, errbuf);
#endif

			// FIXME out of range may happen here
			Throw((char *)errbuf, MException::ME_RUNTIME);

			break;
		case OCI_INVALID_HANDLE:
#ifdef DEBUG_MDB
			(void) printf("Error - OCI_INVALID_HANDLE\n");
#endif

			Throw("OCI_INVALID_HANDLE", MException::ME_RUNTIME);

			break;
		case OCI_STILL_EXECUTING:
#ifdef DEBUG_MDB
			(void) printf("Error - OCI_STILL_EXECUTE\n");
#endif
			Throw("OCI_STILL_EXECUTE", MException::ME_RUNTIME);
			break;

		case OCI_CONTINUE:
#ifdef DEBUG_MDB
			(void) printf("Error - OCI_CONTINUE\n");
#endif
			Throw("OCI_CONTINUE", MException::ME_RUNTIME);
			break;
		default:
#ifdef DEBUG_MDB
			printf("Error - unknown status code: %d\n",status_);
#endif
			Throw("unknown status code", MException::ME_RUNTIME);
			break;
	}
}
