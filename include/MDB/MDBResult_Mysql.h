/**********************************************************
 * Filename:    MDBResult_Mysql.h
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

#ifndef _INCLUDE_MDBRESULT_MYSQL_H_
#define _INCLUDE_MDBRESULT_MYSQL_H_

#include <string>

#include <mysql/mysql.h>

#include "MDBResult.h"
#include "MDBRow.h"
#include "MDBColumn.h"

namespace MDB
{
	/*!
	 * @class MDBResult_Mysql
	 * @brief MDBResult的MySql实现
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-14
	 *
	 * 针对MySql数据库，实现了 MDBResult 定义的接口。
	 *
	 */
	class MDBResult_Mysql : public MDBResult
	{
		public:
			/*!
			 * @brief 构造函数 
			 *
			 * @param [in] mysql_ mysql连接句柄
			 * @param [in] res_ mysql查询结果集
			 *
			 */
			MDBResult_Mysql(MYSQL* mysql_, MYSQL_RES * res_);

			/*!
			 * @brief 析构函数
			 *
			 *
			 */
			~MDBResult_Mysql();

			/*!
			 * @brief 从结果集中取出一行
			 *
			 * @return 
			 *   -- MDBRow指针
			 */
			MDBRow* FetchRow();

			/*
			 * @brief 在结果集中移动行标
			 *
			 * @param [in] 结果集偏移量
			 */
			void Seek(long offset_);

			/*!
			 * @brief 获取结果集的列数
			 *
			 * @return
			 *   -- 当前结果集列数
			 */
			long NumCols();

			/*!
			 * @brief 获取结果集行数
			 *
			 * @return
			 *   -- 当前结果集行数
			 *
			 * @bug 未实现
			 */
			long NumRows();

			long RowCount();

			/*!
			 * @brief 释放所有资源
			 *
			 */
			void Free();

		private:

			//! mysql连接句柄
			MYSQL *mysql_m;

			//! mysql结果集句柄
			MYSQL_RES *res_m;

			/*!
			 * @brief 转换列类型
			 *
			 * 由MDB自定义列类型转换为mysql定义列类型
			 *
			 * @param [in] type_ MDB自定义列类型
			 *
			 */
			ColumnType TransColumnType(int type_);

			/*!
			 * @brief 数据类型转换
			 *
			 * 由于使用mysql api取出的数据都是char *类型的
			 * 若要有效使用，必须进行类型转换变为相对应的类型
			 * 
			 * @param [in] data_ mysql行数据
			 * @param [in] column_ 列对象指针
			 */
			void TransData(const char *data_, MDBColumn *column_);

	};
}

#endif /* _INCLUDE_MDBRESULT_MYSQL_H_ */
