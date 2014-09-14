/**********************************************************
 * Filename:    MDBRow.cpp
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
#include "MDB/MDBRow.h"

MDB::MDBRow:: MDBRow(MDBField** fields_, int numcols_ )
	: field_m(fields_), numcols_m(numcols_)
{

}

MDB::MDBRow:: ~MDBRow()
{
	for(int i=0; i< numcols_m; i++)
	{
		delete field_m[i];	
	}

	delete [] field_m;
}


MDB::MDBField& MDB::MDBRow:: operator [] (int index_)
{
	if(index_ >= numcols_m || index_ < 0)
	{
		Throw("Row index out of range", MException::ME_OUTRANGE);
	}

	return **(field_m + index_);
}
