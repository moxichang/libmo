/**********************************************************
 * Filename:    MDBStatement_Oracle.cpp
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

#include <string>
#include <oci.h>

#include "MDB/MDBStatement_Oracle.h"
#include "MDB/MDBResult_Oracle.h"
#include "MDB/MDBDate.h"

MDB::MDBStatement_Oracle:: MDBStatement_Oracle(OCIStmt * stmthp_, OCIError *errhp_)
	: stmthp_m(stmthp_), errhp_m(errhp_)
{

}

MDB::MDBStatement_Oracle:: ~MDBStatement_Oracle()
{

	if(stmthp_m)
	{
		OCIHandleFree((dvoid *) stmthp_m, (CONST ub4) OCI_HTYPE_STMT);
	}

	if(errhp_m)
	{
		OCIHandleFree((dvoid *) errhp_m, (CONST ub4) OCI_HTYPE_ERROR);
	}

	std::vector<struct Parameter *>::iterator it;

	for(it = parameters.begin(); it != parameters.end(); it++)
	{
		delete (*it);
	}
}

void* MDB::MDBStatement_Oracle:: GetStatmentHandler() const
{
	return stmthp_m;
}

void MDB::MDBStatement_Oracle:: BindByName(std::string name_, ColumnType type_, const void* data_, int data_length_)
{
	OCIBind  *bndhp;
	ub2 oracle_type;

	// FIXME
	oracle_type = MDBResult_Oracle::GetOracleType(type_);

	if(oracle_type == SQLT_DAT)
	{
		MDBDate *date = (MDBDate *)data_;
		unsigned char *buf = new unsigned char[7];

		// 世纪
		buf[0] = (date->Year() / 100) + 100;
		// 年
		buf[1] = (date->Year() % 100) + 100;
		// 月
		buf[2] = date->Month();
		// 日
		buf[3] = date->Day();
		// 小时
		buf[4] = date->Hour();
		// 分钟
		buf[5] = date->Minute();
		// 秒
		buf[6] = date->Second();

		data_ = buf;
		data_length_ = 7;
	}

	struct Parameter *para = new Parameter(name_, type_, data_, data_length_);

	parameters.push_back( para );

	name_ = ":" + name_;

	OCIBindByName(stmthp_m, &bndhp, errhp_m, (text *) name_.c_str(),
			-1, (dvoid *) data_, (sb4) data_length_, 
			oracle_type, (dvoid *) &(para->indicator),
			(ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

}

void MDB::MDBStatement_Oracle:: BindByPos(int pos_, ColumnType type_, const void* data_, int data_length_)
{
	OCIBind  *bndhp;
	ub2 oracle_type;

	if(pos_ <= 0)
	{
		//FIXME
	}

	// FIXME
	oracle_type = MDBResult_Oracle::GetOracleType(type_);

	if(oracle_type == SQLT_DAT)
	{
		MDBDate *date = (MDBDate *)data_;
		char *buf = new char[7];

		// 世纪
		buf[0] = (date->Year() / 100) + 100;
		// 年
		buf[1] = (date->Year() % 100) + 100;
		// 月
		buf[2] = date->Month();
		// 日
		buf[3] = date->Day();
		// 小时
		buf[4] = date->Hour() + 1;
		// 分钟
		buf[5] = date->Minute() + 1;
		// 秒
		buf[6] = date->Second() + 1;

		data_ = buf;
		data_length_ = 7;
	}

	struct Parameter *para = new Parameter(pos_, type_, data_, data_length_);

	//FIXME if the OICBindByPosFailed, got to delete the parameter?
	parameters.push_back( para );

	OCIBindByPos(stmthp_m, &bndhp, errhp_m, (ub4) pos_,
			(dvoid *) data_, (sb4) data_length_,
			oracle_type, (dvoid *) &(para->indicator),
			(ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

}

