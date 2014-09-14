/**********************************************************
 * Filename:    MDBDriver_Mysql.h
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

#ifndef _INCLUDE_MDBBRIVER_MYSQL_H_
#define _INCLUDE_MDBBRIVER_MYSQL_H_

#include <string>

#include <mysql/mysql.h>

#include "MDBDriver.h"
#include "MDBStatement.h"
#include "MDBResult.h"
#include "MDBColumn.h"

namespace MDB
{
	/*!
	 * @class MDBDriver_Mysql
	 * @brief MDBDriver 的Mysql实现
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-14
	 *
	 * MDBDriver 的子类，实现了对于MySql数据库的查询与事务操作
	 *
	 * @todo 
	 */
	class MDBDriver_Mysql : public MDBDriver
	{
		public:
			/*!
			 * @brief 构造函数
			 * 
			 * 初始化MySql API环境，连接数据库 \n
			 * 
			 * @param[in] username_ 数据库用户名
			 * @param[in] password_ 密码
			 * @param[in] host_ 数据库主机地址
			 * @param[in] database_ 数据库名
			 * @param[in] port_ 端口 默认值为3306
			 *
			 */
			MDBDriver_Mysql(std::string username_,
					std::string password_,
					std::string host_,
					std::string database_,
					int port_ = 3306);

			/*!
			 * @brief 析构函数
			 *
			 * 删除句柄资源并断开连接
			 */
			~MDBDriver_Mysql();

			/*!
			 * @brief 连接数据库 
			 *
			 */
			void Connect();

			/*!
			 * @brief 断开数据库连接 
			 *
			 */
			void Disconnect();

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
			MDBResult* Query(std::string sql_);

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
			MDBResult* Query(const MDBStatement* stm_);

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
			unsigned long long Execute(std::string sql_);

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
			unsigned long long Execute(const MDBStatement* stm_);

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
			MDBStatement* Prepare(std::string sql_);

			/*!
			 * @brief 回滚事务
			 *
			 * 撤销当前连接所做的修改
			 */
			void Commit();

			/*!
			 * @brief 获取数据源描述字符串
			 *
			 * @return
			 *    -- 数据源描述字符串
			 */
			void Rollback();


		private:

			MYSQL *mysql; /*!< mysql服务器句柄 */

			/*!
			 * @brief 初始化编程环境
			 *
			 */
			void InitEnviorment();

	};
}

#endif /* _INCLUDE_MDBBRIVER_MYSQL_H_ */
