/**********************************************************
 * Filename:    MDBStatement.h
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

#ifndef _INCLUDE_MDBSTATEMENT_H_
#define _INCLUDE_MDBSTATEMENT_H_

#include <string>

#include "MDBColumn.h"

namespace MDB
{
	/*!
	 * @class MDBStatement
	 * @brief 预查询对象类
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-14
	 *
	 * 预查询对象基类，提供参数绑定功能，用于屏蔽各个数据库系统间的差异。
	 *
	 */
	class MDBStatement
	{
		public:
			/*!
			 * @brief 构造函数 
			 *
			 */
			MDBStatement();

			/*!
			 * @brief 析构函数 
			 *
			 */
			virtual ~MDBStatement();

			/*!
			 * @brief 获取Statment对象
			 *
			 * @return 
			 *   -- 指向对象的指针
			 */
			virtual void* GetStatmentHandler() const = 0;

			/*!
			 * @brief 按名称绑定参数
			 *
			 * sql语句的参数格式为":id"（名称前加冒号）
			 *
			 * @param[in] name_ 参数名
			 * @param[in] type_ 参数类型
			 * @param[in] data_ 指向参数数据的指针
			 * @param[in] data_length_ 数据长度
			 */
			virtual void BindByName(std::string name_, ColumnType type_, const void* data_, int data_length_) = 0;

			/*!
			 * @brief 按位置绑定参数
			 *
			 * sql语句的参数格式为":2"（名称前加冒号）
			 *
			 * @param[in] pos_ 参数位置
			 * @param[in] type_ 参数类型
			 * @param[in] data_ 指向参数数据的指针
			 * @param[in] data_length_ 数据长度
			 *
			 */
			virtual void BindByPos(int pos_, ColumnType type_, const void* data_, int data_length_) = 0;

	};
}

#endif /* _INCLUDE_MDBSTATEMENT_H_ */
