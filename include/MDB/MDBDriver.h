/**********************************************************
 * Filename:    MDBDriver.h
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

#ifndef _INCLUDE_MDBDRIVER_H_
#define _INCLUDE_MDBDRIVER_H_

#include <string>

#include "MDBStatement.h"
#include "MDBResult.h"

namespace MDB
{
	/*!
	 * @class MDBDriver
	 * @brief 数据库驱动基类
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-3
	 *
	 * MDBDriver基类，定义了连接、断开数据库，Query，Prepare
	 * 提交事务(commit)，回滚(commit)等操作。
	 *
	 * 此类为纯虚类，针对不同的数据库从此类派生子类，在子类中实现具体功能。
	 *
	 * @todo 
	 */
	class MDBDriver
	{
		public:

			/*!
			 * @brief 构造函数
			 * 
			 * 将参数中给定的值保存到成员变量中，除此外不做任何操作
			 * 
			 * @param[in] username_ 数据库用户名
			 * @param[in] password_ 密码
			 * @param[in] database_ 数据库名
			 * @param[in] host_ 数据库所在主机
			 * @param[in] port_ 端口 若为0，使用默认端口
			 */
			MDBDriver(std::string username_,
					std::string password_,
					std::string database_,
					std::string host_,
					int port_ = 0
				 );

			/*!
			 * @brief 析构函数
			 *
			 */
			virtual ~MDBDriver();

			/*!
			 * @brief 连接数据库
			 *
			 * 虚函数，等待子类实现
			 */
			virtual void Connect() = 0;

			/*!
			 * @brief 断开数据库连接
			 *
			 * 虚函数，等待子类实现
			 */
			virtual void Disconnect() = 0;

			/*!
			 * @brief 执行查询字符串
			 *
			 * 用于执行返回结果集的查询（select）
			 *
			 * @param[in] sql_ 查询字符串
			 *
			 * @return
			 *    -- 结果集指针 若查询无数据，返回NULL
			 */
			virtual MDBResult* Query(std::string sql_) = 0;

			/*!
			 * @brief 执行 MDBStatement 查询对象
			 *
			 * 用于执行返回结果集的查询（select）
			 *
			 * @param[in] stm_ MDBStatement 对象
			 *
			 * @return
			 *    -- 结果集指针 若查询无数据，返回NULL
			 */
			virtual MDBResult* Query(const MDBStatement* stm_) = 0;

			/*!
			 * @brief 执行查询字符串
			 *
			 * 用于执行非select查询（update, insert, delete）
			 *
			 * @param[in] sql_ 查询字符串
			 *
			 * @return
			 *    --  查询影响的行数
			 */
			virtual unsigned long long Execute(std::string sql_) = 0;

			/*!
			 * @brief 执行 MDBStatement 查询对象
			 *
			 * 用于执行非select查询（update, insert, delete）
			 *
			 * @param[in] stm_ MDBStatement 对象
			 *
			 * @return
			 *    --  查询影响的行数
			 */
			virtual unsigned long long Execute(const MDBStatement* stm_) = 0;

			/*!
			 * @brief 为执行操作准备SQL字符串
			 *
			 * 预格式化sql语句，以绑定参数
			 *
			 * @param[in] sql_ 查询字符串
			 *
			 * @return
			 *    --  MDBStatement对象
			 */
			virtual MDBStatement* Prepare(std::string sql_) = 0;

			/*!
			 * @brief 提交当前事务
			 *
			 * 将当前连接所做的修改写入数据库
			 */
			virtual void Commit() = 0;

			/*!
			 * @brief 回滚事务
			 *
			 * 撤销当前连接所做的修改
			 */
			virtual void Rollback() = 0;

			/*!
			 * @brief 获取数据源描述字符串
			 *
			 * @return
			 *    -- 数据源描述字符串
			 */
			std::string GetDSN();

		protected:

			//! 数据库用户名
			std::string username_m;

			//! 密码
			std::string password_m;

			//! 数据库名
			std::string database_m;

			//! 主机地址
			std::string host_m;

			//! 端口号
			int port_m;
	};
}

#endif /* _INCLUDE_MDBDRIVER_H_ */
