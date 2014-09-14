/**********************************************************
 * Filename:    MDBResult.cpp
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

#include "MDB/MDBResult.h"

MDB::MDBResult:: MDBResult()
	: numcols_m(0), cursor_m(0), numrows_m(0) 
{

}

MDB::MDBResult:: ~MDBResult()
{
	// 删除列对象
	for(int i=0; i < numcols_m; i++ )
	{
		delete columns_m[i];
	}

	// 删除列对象数组
	delete [] columns_m;

	// 删除行缓存
	// MDBRow的析构函数设置为私有，同时声明MDBResult类为MDBRow的友元类
	// 以此阻止用户自行删除MDBRow对象
	for(RowMap::iterator it = rowmap_m.begin(); it != rowmap_m.end(); it++)
	{
		delete it->second;
	}
}

