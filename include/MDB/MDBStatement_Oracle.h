/**********************************************************
 * Filename:    MDBStatement_Oracle.h
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

#ifndef _INCLUDE_MDBSTATEMENT_ORACLE_H_
#define _INCLUDE_MDBSTATEMENT_ORACLE_H_

#include <string>
#include <vector>

#include "MDBColumn.h"
#include "MDBStatement.h"

namespace MDB
{
	/*!
	 * @class MDBStatement_Oracle
	 * @brief MDBStatement 类的Oracle实现
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-14
	 *
	 * 预查询对象派生类，针对Oracle实现
	 *
	 */
	class MDBStatement_Oracle : public MDBStatement
	{

		public:
			/*!
			 * @brief 构造函数 
			 *
			 * @param[in] stmthp_ OCI statement句柄
			 * @param[in] errhp_ OCI 错误处理句柄
			 *
			 */
			MDBStatement_Oracle(OCIStmt * stmthp_, OCIError *errhp_);

			/*!
			 * @brief 析构函数 
			 *
			 */
			~MDBStatement_Oracle();

			/*!
			 * @brief 获取Statment对象
			 *
			 * @return 
			 *   -- 指向对象的指针
			 */
			void* GetStatmentHandler() const;

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
			void BindByName(std::string name_, ColumnType type_, const void* data_, int data_length_);

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
			void BindByPos(int pos_, ColumnType type_, const void* data_, int data_length_);

		private:
			//! OCI statement句柄
			OCIStmt *stmthp_m;

			//! OCI错误处理句柄
			OCIError *errhp_m;

			struct Parameter
			{
				std::string name;
				int position;
				ColumnType type;

				const void* data;
				int data_length;

				short indicator;

				Parameter(std::string name_, ColumnType type_, const void* data_, int data_length_)
					: name(name_), type(type_), 
					data(data_), data_length(data_length_),
					indicator(0)
				{}


				Parameter(int position_, ColumnType type_, const void* data_, int data_length_)
					: position(position_), type(type_), 
					data(data_), data_length(data_length_),
					indicator(0)
				{}

				// FIXME!!
				// Is there any better way?
				~Parameter()
				{
					if(type == DATE)
					{
						delete [] (char *)data;
					}
				}
			};

			std::vector<struct Parameter *> parameters;
	};
}

#endif /* _INCLUDE_MDBSTATEMENT_ORACLE_H_ */
