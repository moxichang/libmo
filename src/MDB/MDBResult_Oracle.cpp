/**********************************************************
 * Filename:    MDBResult_Oracle.cpp
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

#include "Log.h"
#include "Exception.h"
#include "MDB/MDBResult_Oracle.h"

MDB::MDBResult_Oracle:: MDBResult_Oracle(OCIStmt *stmthp_, OCIError *errhp_, MDBColumn **columns_, ub2 numcols_)
	: MDBResult()
{
	if(stmthp_ == NULL || errhp_ == NULL || columns_ == NULL)
	{
		Throw("Construct MDBResult_Oracle object failed, some arguments passed as null", MException::ME_INVARG);
	}

	stmthp_m = stmthp_;
	errhp_m = errhp_;
	columns_m = columns_;
	numcols_m = numcols_;

	cursor_m = cursor_m +1;

	OCIDefine *defnp = NULL;

	/*!
	 * 循环对输出列进行define
	 *
	 * @bug FIXME
	 * 1. 在这里多列共用一个 OCIDefine 句柄指针，会不会有问题？ \n
	 * 2. 手册中说如果defnp 为NULL的话，OCIDefineByPos会为其隐式分配空间 \n
	 *    但没说是否需要自己释放，是否要手动释放？搞清这点，避免内存泄漏 \n
	 *
	 * 3. 输出缓冲要求两字节对齐, 现在这个还没完成 \n
	 *
	 * Manual URL: \n 
	 * http://download.oracle.com/docs/cd/B14117_01/appdev.101/b10779/oci15re5.htm#449680 \n
	 * http://download.oracle.com/docs/cd/B14117_01/appdev.101/b10779/oci05bnd.htm#421279 \n
	 */
	for(int i=1; i <= numcols_m; i++ ){

		sb4 size = columns_m[i-1]->type_size;
		ub2 ora_type = GetOracleType(columns_m[i-1]->type);

		CheckError(OCIDefineByPos (stmthp_m, &defnp, errhp_m,
				i, (dvoid *)columns_m[i-1]->data, size, ora_type,
				(dvoid *) &(columns_m[i-1]->indicator),  (ub2 *) 0,  (ub2 *) 0,  OCI_DEFAULT));

	}

}

MDB::MDBResult_Oracle:: ~MDBResult_Oracle()
{
	Free();
}

MDB::MDBRow* MDB::MDBResult_Oracle:: FetchRow()
{
	sword status;

	if(cursor_m == -1)
	{
#ifdef MDB_DEBUG
		LOG(LOG_DEBUG, "Out of range.");
#endif

		return NULL;
	}

	RowMap::iterator it;

	// 首先在行缓存中查找
	if( (it = rowmap_m.find( cursor_m )) != rowmap_m.end())
	{
		cursor_m++;
		return it->second;
	}

	// 行缓存中不存在，通过OCI接口获取
	if ((status = OCIStmtFetch2 (stmthp_m, errhp_m, 1, OCI_FETCH_ABSOLUTE,(sb4) cursor_m, OCI_DEFAULT)))
	{
		if(status == OCI_NO_DATA)
		{
			cursor_m = -1;
			return NULL;
		}
		else{
			CheckError(status);
			return NULL;
		}
	}
	else
	{

		MDBField **fields = new MDBField*[numcols_m];

		for(int i=0; i< numcols_m; i++)
		{
			if(columns_m[i]->type == DATE)
			{
				unsigned char *ora_date = (unsigned char *)columns_m[i]->data;

				int year = (ora_date[0] - 100)*100 + (ora_date[1] - 100);

#ifdef MDB_DEBUG
				printf("%u%u-%u-%u %02u:%02u:%02u\n",
					ora_date[0]-100, ora_date[1]-100,
					ora_date[2],ora_date[3],
					ora_date[4] - 1,ora_date[5] - 1,ora_date[6] - 1);
#endif
				MDBDate date(year, ora_date[2], ora_date[3],
					ora_date[4] - 1, ora_date[5] - 1, ora_date[6] - 1);

				fields[i] = new MDBField(columns_m[i]->type, &date, sizeof(MDBDate) );
			}
			else
			{
				fields[i] = new MDBField(columns_m[i]->type, columns_m[i]->data, columns_m[i]->type_size );
			}
		}
		
		// 创建MDBRow对象
		MDBRow *nrow = new MDBRow(fields, numcols_m);

		// 放入行缓存
		rowmap_m[cursor_m] = nrow;

		cursor_m++;

		return nrow;

	}

	return NULL;
}

void MDB::MDBResult_Oracle:: Seek(long offset_)
{

	if(offset_ < 0)
	{
		Throw("Offset given out of range", MException::ME_INVARG);
	}

	// Oracle行号从1开始，MDB默认从0开始，因此需要加1
	cursor_m = offset_ + 1;
}

long MDB::MDBResult_Oracle:: NumCols()
{
	return numcols_m;
}

long MDB::MDBResult_Oracle:: NumRows()
{
	return 0;
}

long MDB::MDBResult_Oracle:: RowCount()
{
	return 0;
}

void MDB::MDBResult_Oracle:: Free()
{
	if(stmthp_m) OCIHandleFree((dvoid *) stmthp_m, (CONST ub4) OCI_HTYPE_STMT);
	if(errhp_m) OCIHandleFree((dvoid *) errhp_m, (CONST ub4) OCI_HTYPE_ERROR);
}

ub2 MDB::MDBResult_Oracle:: GetOracleType(ColumnType type_)
{
	ub2 ora_type;

	switch(type_)
	{
		case NUMBER:
			ora_type = SQLT_FLT;
#ifdef MDB_DEBUG
			LOG(LOG_DEBUG,"SQLT_INT");
#endif
			break;

		case INTEGER:
			ora_type = SQLT_INT;
#ifdef MDB_DEBUG
			LOG(LOG_DEBUG,"SQLT_INT");
#endif
			break;

		case FLOAT:
			ora_type = SQLT_FLT;
#ifdef MDB_DEBUG
			LOG(LOG_DEBUG,"SQLT_FLT");
#endif
			break;

		case VARCHAR:
			ora_type = SQLT_STR;
#ifdef MDB_DEBUG
			LOG(LOG_DEBUG,"SQLT_STR");
#endif
			break;

		case DATE:
			ora_type = SQLT_DAT;
#ifdef MDB_DEBUG
			LOG(LOG_DEBUG,"SQLT_STR");
#endif
			break;

		default:
			//FIXME!!
#ifdef MDB_DEBUG
			LOG(LOG_DEBUG,"SQLT_STR");
#endif
			ora_type = SQLT_STR;
	}	

	return ora_type;
}

void MDB::MDBResult_Oracle:: CheckError(sword status_)
{
	/*!
	 * @bug 512字节的errbuf是否足够？
	 */
	text errbuf[512];
	sb4 errcode = 0;

	switch (status_)
	{
		case OCI_SUCCESS:
			break;
		case OCI_SUCCESS_WITH_INFO:
			(void) printf("Error - OCI_SUCCESS_WITH_INFO\n");
			break;
		case OCI_NEED_DATA:
			(void) printf("Error - OCI_NEED_DATA\n");
			break;
		case OCI_NO_DATA:
			(void) printf("Error - OCI_NODATA\n");
			break;
		case OCI_ERROR:
			(void) OCIErrorGet((dvoid *)errhp_m, (ub4) 1, (text *) NULL, &errcode,
					errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
			(void) printf("Error - %.*s\n", 512, errbuf);
			break;
		case OCI_INVALID_HANDLE:
			(void) printf("Error - OCI_INVALID_HANDLE\n");
			break;
		case OCI_STILL_EXECUTING:
			(void) printf("Error - OCI_STILL_EXECUTE\n");
			break;
		case OCI_CONTINUE:
			(void) printf("Error - OCI_CONTINUE\n");
			break;
		default:
			break;
	}
}
