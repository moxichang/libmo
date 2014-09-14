/**********************************************************
 * Filename:    MDBColumn.h
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

#ifndef _INCLUDE_MDBCOLUMN_H_
#define _INCLUDE_MDBCOLUMN_H_

#include <string>

namespace MDB
{
	//! 列类型枚举
	enum ColumnType {
		NUMBER,   /**< 对应于Oracle的Number类型 */
		INTEGER,  /**< 整型 4字节 */
		FLOAT,    /**< 浮点数 4字节 */
		VARCHAR,  /**< 字符串 */
		DATE,     /**< 日期 */
		UNKNOWN   /**< 未知 */
	};

	/*!
	 * @class MDBColumn
	 * @brief 数据列抽象类
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-3
	 *
	 * 数据列抽象类。
	 *
	 * @bug
	 *	为了读取方便，类成员都是公有的。
	 */
	class MDBColumn
	{
		public:
			/*!
			 * @brief 构造函数
			 *
			 * @param[in] type_ 列类型
			 * @param[in] name_  列名
			 * @param[in] length_  列长度
			 *
			 */
			MDBColumn(ColumnType type_, std::string name_, long length_);

			/*!
			 * @brief 析构函数
			 *
			 */
			~MDBColumn();

			//! 列类型
			ColumnType type;

			//! 列类型长度
			int type_size;

			//! 列名
			std::string name;

			//! 列实际长度（列类型为VARCAR时，此字段有效）
			long length;

			//! 列数据（供OCI Fetch时使用）
			char *data;

			//! 列指示符（供OCI Fetch时使用，检测列是否为null）
			short indicator;

			/*!
			 * @brief 获取列类型大小
			 *
			 */
			int GetTypeSize();
	};
}

#endif /* _INCLUDE_MDBCOLUMN_H_ */
