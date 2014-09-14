/**********************************************************
 * Filename:    MDBResult_Oracle.h
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

#ifndef _INCLUDE_MDBRESULT_ORACLE_H_
#define _INCLUDE_MDBRESULT_ORACLE_H_

#include <string>

#include <oci.h>

#include "MDBResult.h"
#include "MDBRow.h"
#include "MDBColumn.h"

namespace MDB
{
	/*!
	 * @class MDBResult_Oracle
	 * @brief MDBResult 的Oracle实现
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-14
	 *
	 * 针对Oracle数据库，实现了 MDBResult 定义的接口。
	 *
	 * 
	 *
	 */
	class MDBResult_Oracle : public MDBResult
	{
		public:
			/*!
			 * @brief 构造函数 
			 *
			 * @param [in] stmthp_ OCI statment查询句柄
			 * @param [in] envhp_  OIC 环境句柄
			 * @param [in] columns_ 列对象数组
			 * @param [in] numcols_ 列对象数组长度
			 *
			 */
			MDBResult_Oracle(OCIStmt *stmthp_, OCIError *envhp_, MDBColumn **columns_, ub2 numcols_);

			/*!
			 * @brief 析构函数
			 *
			 *
			 */
			~MDBResult_Oracle();

			/*!
			 * @brief 从结果集中取出一行
			 *
			 * @return 
			 *   -- MDBRow指针
			 *
			 * @bug 
			 *    引用还是指针？
			 *    是否要使用参数？
			 *       1、fetch特定行
			 *       2、顺序fetch
			 */
			MDBRow* FetchRow();

			/*
			 * @brief 在结果集中移动行标
			 *
			 * 由于Oracle中的结果集行数是从1开始的，
			 * 所以在传递到函数内部的 cursor_m 变量后需要做加1操作
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

			/*!
			 * @brief 转换列类型
			 *
			 * 由MDB自定义列类型转换为OCI定义列类型
			 *
			 * @param [in] type_ MDB自定义列类型
			 *
			 */
			static ub2 GetOracleType(ColumnType type_);

		private:

			//! OCI Statement句柄
			OCIStmt *stmthp_m;

			//! OCI错误句柄
			OCIError *errhp_m;

			/*! 
			 * @brief 错误检查函数
			 *
			 * 检查OCI函数返回值是否正确，若不正确，抛出相应异常
			 *
			 * @param [in] status_ OCI函数返回状态代码
			 *
			 */
			void CheckError(sword status_);

	};
}

#endif /* _INCLUDE_MDBRESULT_ORACLE_H_ */
