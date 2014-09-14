/**********************************************************
 * Filename:    MDBColumn.cpp
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

#include <string.h>

#include "MDB/MDBColumn.h"

MDB::MDBColumn:: MDBColumn(ColumnType type_, std::string name_, long length_)
	: type(type_), name(name_), length(length_) , data(0)
{
	type_size = GetTypeSize();

	data = new char[type_size];
	memset(data, 0, type_size);
}

MDB::MDBColumn:: ~MDBColumn()
{
	delete [] data;
}

int MDB::MDBColumn:: GetTypeSize()
{

	int size = -1;

	switch(type)
	{
		case NUMBER:
			size = sizeof(int);
			break;

		case INTEGER:
			size = sizeof(int);
			break;

		case FLOAT:
			size = sizeof(float);
			break;

		case VARCHAR:
			size = length + 1;
			break;

		case DATE:
			//FIXME
			// Date类型，Oracle按7字节读取
			// mysql按字符串存储，因此给个比较大的空间
			size = 32;
			break;

		default:
			//FIXME!!
			size = -1;
	}	

	return size;
}

