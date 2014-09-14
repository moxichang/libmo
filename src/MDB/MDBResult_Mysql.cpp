/**********************************************************
 * Filename:    MDBResult_Mysql.cpp
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

#include <mysql/mysql.h>

#include "Exception.h"

#include "MDB/MDBDate.h"
#include "MDB/MDBResult_Mysql.h"

MDB::MDBResult_Mysql:: MDBResult_Mysql(MYSQL* mysql_, MYSQL_RES * res_)
	: MDBResult(), mysql_m(mysql_), res_m(res_)
{
	if(mysql_ == NULL || res_ == NULL)
	{
		Throw("NULL argument detected",MException::ME_INVARG);
	}

	numcols_m = mysql_field_count(mysql_m);

	columns_m = new MDBColumn*[numcols_m];

	for(int i=0; i< numcols_m; i++)
	{
		columns_m[i] = NULL;
	}

	try{
		int i = 0;
		MYSQL_FIELD *field;

		while((field = mysql_fetch_field(res_m)))
		{
			// FIXME
			// 这里可作优化
			columns_m[i] = new MDBColumn( TransColumnType(field->type), field->name, field->max_length);
			i++;
		}
	}
	catch(std::exception &e)
	{
		for(int i=0; i< numcols_m; i++)
		{
			delete columns_m[i];
		}
		
		delete [] columns_m;

		Throw(e.what(), MException::ME_RETHROW);
	}

}

MDB::MDBResult_Mysql:: ~MDBResult_Mysql()
{
	Free();
}

MDB::MDBRow* MDB::MDBResult_Mysql:: FetchRow()
{
	MYSQL_ROW row;

	// 查找行缓存中是否存在
	RowMap::iterator it ;
	if( ( it = rowmap_m.find( cursor_m )) != rowmap_m.end())
	{
		++cursor_m;

		mysql_data_seek(res_m, cursor_m);

		return it->second;
	}

	// 行缓存中不存在，调用数据库接口获取
	if((row = mysql_fetch_row(res_m)) != NULL)
	{
		MDBField **fields = new MDBField*[numcols_m];

		for(int i=0; i< numcols_m ; i++)
		{
			if( row[i] != NULL)
			{
				TransData(row[i], columns_m[i]);
				fields[i] = new MDBField(columns_m[i]->type, columns_m[i]->data, columns_m[i]->length);
			}
			else
			{
				fields[i] = new MDBField(columns_m[i]->type, NULL, 0);
			}
		}

		// 创建MDBRow对象
		MDBRow *nrow = new MDBRow(fields, numcols_m);

		// 入行缓存
		rowmap_m[cursor_m] = nrow;
		
		++cursor_m;

		return nrow;
	}

	return NULL;
}

void MDB::MDBResult_Mysql:: Seek(long offset_)
{
	if(offset_ < 0)
	{
		Throw("Offset given out of range", MException::ME_INVARG);
	}

	cursor_m = offset_;

	mysql_data_seek(res_m, offset_);
}

long MDB::MDBResult_Mysql:: NumCols()
{
	return numcols_m;
}

long MDB::MDBResult_Mysql:: NumRows()
{
	return mysql_num_rows(res_m);
}

long MDB::MDBResult_Mysql:: RowCount()
{
	return 0;
}

void MDB::MDBResult_Mysql:: Free()
{
	mysql_free_result(res_m);
}

MDB::ColumnType MDB::MDBResult_Mysql:: TransColumnType(int type_)
{
	ColumnType dt;
	
	switch(type_)
	{
		case MYSQL_TYPE_TINY:
	
		case MYSQL_TYPE_SHORT:
	
		case MYSQL_TYPE_LONG:
	
		case MYSQL_TYPE_INT24:
			dt = INTEGER; 
			break;
	
		case MYSQL_TYPE_FLOAT:
	
		case MYSQL_TYPE_DOUBLE:
			dt = FLOAT; 
			break;
	
		//FIXME
		case MYSQL_TYPE_DATETIME:

		case MYSQL_TYPE_DATE:
			dt = DATE; 
			break;
	
		case MYSQL_TYPE_TIMESTAMP:
			dt = INTEGER;
			break;
	
		case MYSQL_TYPE_STRING:
			dt = VARCHAR;
			break;
	
		case MYSQL_TYPE_VAR_STRING:
			dt = VARCHAR;
			break;

		default:
			dt = UNKNOWN;
			Throw("unsupport data type",MException::ME_INVARG);
	}

	return dt;
}

void MDB::MDBResult_Mysql:: TransData(const char *data_, MDBColumn *column_)
{

	switch(column_->type)
	{
		case NUMBER:

		case INTEGER:
		{
			int t = atoi(data_);
			memcpy(column_->data, &t, sizeof(int));
			column_->length = sizeof(int);
			break;
		}

		case FLOAT:
		{
			float t = atof(data_);
			memcpy(column_->data, &t, sizeof(float));
			column_->length = sizeof(float);
			break;
		}

		case VARCHAR:
			// FIXME
			// try to avoid this copy
			sprintf(column_->data, "%s", data_);
			column_->length = strlen(column_->data)+1;
			break;

		case DATE:
		{
			delete column_->data;

			int y = 0;
			int m = 0;
			int d = 0;
			int h = 0;
			int i = 0;
			int s = 0;

			sscanf( data_ ,"%d-%d-%d %d:%d:%d", &y, &m, &d, &h, &i, &s);

			column_->data = (char *)new MDBDate(y, m, d, h, i, s);
			column_->length = sizeof(MDBDate);
			break;
		}

		case UNKNOWN:

		default:
			Throw("unknown data type", MException::ME_INVARG);
	}

}

