/**********************************************************
 * Filename:    MDBResult.h
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

#ifndef _INCLUDE_MDBRESULT_H_
#define _INCLUDE_MDBRESULT_H_

#include <map>

#include "MDBRow.h"
#include "MDBColumn.h"

namespace MDB
{
	/*!
	 * @class MDBResult
	 * @brief 数据库结果集抽象类
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-14
	 *
	 * 数据库结果集抽象类，统一了不同数据库对于结果集的操作方式。
	 * 在本对象中保留对取出的 MDBRow 对象的引用，在最后析构的时候同一删除。\n
	 *
	 * 这样虽然增加了内存占用，但是可以减轻用户的编程负担（FetchRow方法返回的MDBRow对象指针由系统负责释放，用户不必费心维护，减少了内存泄漏发生的概率）
	 * 而且当在结果集中前后移动时，如果读取某之前已经读取过的行，即可在内存中直接引用，不必再去服务器端获取，速度会快一些。
	 *
	 */
	class MDBResult
	{
		public:
			/*!
			 * @brief 构造函数 
			 *
			 */
			MDBResult();

			/*!
			 * @brief 析构函数 
			 *
			 * 释放列对象，列对象数组占用空间
			 * 释放保存的行对象
			 *
			 */
			virtual ~MDBResult();

			/*!
			 * @brief 从结果集中取出一行
			 *
			 * @return 
			 *   -- MDBRow指针
			 */
			virtual MDBRow* FetchRow() = 0;

			/*!
			 * @brief 在结果集中移动行标
			 *
			 * @param[in] offset_ 偏移量
			 */
			virtual void Seek(long offset_) = 0;

			/*!
			 * @brief 获取结果集的列数
			 *
			 * @return
			 *   -- 当前结果集列数
			 */
			virtual long NumCols() = 0;

			/*!
			 * @brief 获取结果集行数
			 *
			 * @return
			 *   -- 当前结果集行数
			 */
			virtual long NumRows() = 0;

			virtual long RowCount() = 0;

			/*!
			 * @brief 释放所有资源
			 *
			 */
			virtual void Free() = 0;

		protected:

			//! 列对象数组
			MDBColumn **columns_m;

			//! 列数
			unsigned short numcols_m;

			typedef std::map<int, MDBRow *> RowMap;

			//! 存储列引用
			std::map<int, MDBRow *> rowmap_m;

			//! 行游标，指向当前所在行
			int cursor_m;

			//! 行数
			int numrows_m;

	};
}

#endif /* _INCLUDE_MDBRESULT_H_ */
