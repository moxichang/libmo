/**********************************************************
 * Filename:    MDBField.cpp
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
#include "MDB/MDBField.h"

#include <string>

#include <stdio.h>
#include <string.h>

MDB::MDBField:: MDBField(ColumnType type_, const void *data_, int length_ )
	: type_m(type_), length_m(length_)
{
	if(data_ == NULL)
	{
		data_m = NULL;
	}
	else {

		if(type_ == DATE){
			data_m = new MDBDate( ((MDBDate *)data_)->Timestamp() );
		}
		else
		{
			//printf("length_ %d\n", length_);
			data_m = new char[length_];
			memcpy(data_m, data_, length_);
		}
	}
}

MDB::MDBField:: ~MDBField()
{
	if(type_m == DATE)
	{
		delete (MDBDate *)data_m;
	}
	else {
		delete [] (char *)data_m;
	}
}

int MDB::MDBField:: AsInt()
{
	if(type_m != INTEGER)
	{
		Throw("Data type mismatch, request for INTEGER, should be " + TransColumnType(type_m), MException::ME_INVARG);
	}

	if(data_m == NULL)
		return 0;

	return *((int *)data_m);
}

float MDB::MDBField:: AsFloat()
{
	if(type_m != FLOAT)
	{
		Throw("Data type mismatch, request for FLOAT, should be " + TransColumnType(type_m), MException::ME_INVARG);
	}
	
	if(data_m == NULL)
		return 0;

	return *((float *)data_m);
}

char* MDB::MDBField:: AsString()
{
	if(type_m != VARCHAR)
	{
		Throw("Data type mismatch, request for VARCHAR, should be " + TransColumnType(type_m), MException::ME_INVARG);
	}

	return (char *)data_m;
}

MDB::MDBDate* MDB::MDBField:: AsDate()
{
	if(type_m != DATE)
	{
		Throw("Data type mismatch, request for DATE, should be " + TransColumnType(type_m), MException::ME_INVARG);
	}

	return (MDBDate *)data_m;
}

bool MDB::MDBField:: IsNull()
{
	return  data_m == NULL ? true : false;
}

std::string MDB::MDBField:: TransColumnType(ColumnType type_)
{
	std::string tname;

	switch(type_)
	{
		case NUMBER:
			tname = "NUMBER";
			break;

		case INTEGER:
			tname = "INTEGER";
			break;

		case FLOAT:
			tname = "FLOAT";
			break;

		case VARCHAR:
			tname = "VARCHAR";
			break;

		case DATE:
			tname = "DATE";
			break;

		default:
			tname = "UNKNOWD";
	}

	return tname;
}


