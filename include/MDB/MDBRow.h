/**********************************************************
 * Filename:    MDBRow.h
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

#ifndef _INCLUDE_MDBROW_H_
#define _INCLUDE_MDBROW_H_

#include "MDBField.h"

namespace MDB
{
	class MDBResult;

	/*!
	 * @class MDBRow
	 * @brief 数据库行抽象类
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-14
	 *
	 * @todo
	 *    是否要添加关联数组读取方式？
	 */
	class MDBRow
	{

		friend class MDBResult;

		public:
			/*!
			 * @brief 构造函数
			 * 
			 * @param[in] fields_ 列对象数组
			 * @param[in] numcols_ 列对象数组长度
			 */
			MDBRow(MDBField** fields_, int numcols_);

			/*!
			 * @brief 重载[]操作符
			 *
			 * 通过下标获取列对象
			 * 
			 * @param[in] index_ 行数组索引
			 */
			MDBField& operator [] (int index_);

		private:
			/*!
			 * @brief 析构函数
			 * 
			 * 析构函数为私有，保证用户不会自行删除 MDBRow 对象
			 * 同时声明 MDBResult 类为 MDBRow 的友元，使得可以再 MDBResult 中删除 MDBRow 对象
			 */
			~MDBRow();

			//! 列对象数组
			MDBField **field_m;

			//! 列数
			int numcols_m;

	};
}

#endif /* _INCLUDE_MDBROW_H_ */
