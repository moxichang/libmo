/**********************************************************
 * Filename:    MDBField.h
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

#ifndef _INCLUDE_MDBFIELD_H_
#define _INCLUDE_MDBFIELD_H_

#include "MDBColumn.h"
#include "MDBDate.h"

namespace MDB
{
	/*!
	 * @class MDBField
	 * @brief 数据字段抽象类
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-3
	 *
	 * 数据字段抽象类，提供按各种类型读取的方法。
	 *
	 * @bug 
	 *	1、当字段对应于Oracle的Number列类型而且值的大小超出float的表示范围时，将无法正确读取其数值。
	 *	2、IsNull() 方法未实现
	 *
	 */
	class MDBField
	{
		public:
			/*!
			 * @brief 构造函数 
			 *
			 * @param[in] type_ 字段类型 @see MDB::MDBColumn
			 * @param[in] data_ 字段数据
			 * @param[in] length_ 字段数据长度
			 *
			 */
			MDBField(ColumnType type_, const void *data_, int length_);

			/*!
			 * @brief 析构函数
			 *
			 */
			~MDBField();

			/*!
			 * @brief 以类型返回字段数据
			 *
			 * @return 
			 *   -- int 型表示的字段数据 
			 */
			int AsInt();
			
			/*!
			 * @brief 以类型返回字段数据
			 *
			 * @return 
			 *   -- float 型表示的字段数据
			 */
			float AsFloat();
			
			/*!
			 * @brief 以类型返回字段数据
			 *
			 * @return 
			 *   -- char* 型表示字段列数据
			 */
			char* AsString();
			
			/*!
			 * @brief 以类型返回字段数据
			 *
			 * @return 
			 *   -- MDBDate 型表示字段数据
			 */
			MDBDate* AsDate();

			/*!
			 * @brief 判断字段是否为null
			 *
			 * @return
			 *   -- true 当前字段为null
			 *   -- false 当前字段不为null
			 */
			bool IsNull();

			/*!
			 * @brief 将列类型翻译成字符串
			 *
			 * @param[in] type_ 列类型枚举
			 *
			 * @return
			 *   -- 列类型名
			 */
			std::string TransColumnType(ColumnType type_);

		private:

			//! 字段类型
			ColumnType type_m;

			//! 字段数据
			void *data_m;

			//! 字段数据长度
			int length_m;
	};
}

#endif /* _INCLUDE_MDBFIELD_H_ */
