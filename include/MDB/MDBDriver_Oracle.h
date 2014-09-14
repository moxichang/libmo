/**********************************************************
 * Filename:    MDBDriver_Oracle.h
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

#ifndef _INCLUDE_MDBBRIVER_ORACLE_H_
#define _INCLUDE_MDBBRIVER_ORACLE_H_

#include <string>

#include <oci.h>

#include "MDBDriver.h"
#include "MDBStatement.h"
#include "MDBResult.h"
#include "MDBColumn.h"

namespace MDB
{
	/*!
	 * @class MDBDriver_Oracle
	 * @brief MDBDriver 的Oracle实现
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-14
	 *
	 * MDBDriver 的子类，实现了对于Oracle数据库的查询与事务操作
	 *
	 * @todo
	 *
	 * @bug
	 *	NUMBER列支持不明确
	 *	当NUMBER列的scale为0时，视为INTEGER类型，当不为零时，视为FLOAT类型
	 *	但当NUMBER超过FLOAT能表示的最大值时，将会出错。
	 */
	class MDBDriver_Oracle : public MDBDriver
	{
		public:
			/*!
			 * @brief 构造函数
			 * 
			 * 初始化OCI环境，连接数据库 \n
			 * 
			 * Oracle的主机相关信息（包括主机地址，端口号等）在tsn文件中定义，因此在这里不需要提供。
			 * 
			 * @param[in] username_ 数据库用户名
			 * @param[in] password_ 密码
			 * @param[in] database_ 数据库名
			 *
			 */
			MDBDriver_Oracle(std::string username_,
					std::string password_,
					std::string database_);

			/*!
			 * @brief 析构函数
			 *
			 * 删除句柄资源并断开连接
			 */
			~MDBDriver_Oracle();

			/*!
			 * @brief 连接数据库
			 *
			 * 连接Oracle数据库
			 * OCI中的OCIServerAttach在执行后会屏蔽SIGINT信号
			 * 本函数在连接前保存预设的SIGINT处理句柄，在OCIServerAttach之后恢复，不影响用户代码的信号设置
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

			OCIEnv *envhp;      /*!< 环境句柄 */
			OCIServer *srvhp;   /*!< 服务器句柄 */
			OCISvcCtx *svchp;   /*!< 服务上下文句柄 */
			OCIError *errhp;    /*!< 错误信息句柄 */  
			OCISession *authp;  /*!< 会话句柄 */

			/*!
			 * @brief 初始化OCI环境
			 *
			 */
			void InitEnviorment();

			/*!
			 * @brief 获取列信息
			 *
			 * 在执行bind操作时将需要知道列类型，字段长度等信息，在此一次性获取后传递给 MDBResult_Oracle 对象
			 *
			 * @return 
			 *   -- 返回生成的列对象数组
			 */
			MDBColumn** FetchColumns(OCIStmt *stmthp, ub2& numcols_);

			/*!
			 * @brief 转换Oracle列类型为MDB自定义列类型
			 *
			 * @param[in] dtype oracle列类型代码
			 */
			ColumnType TransColumnType(ub2 dtype);

			/*!
			 * @brief 检查OCI函数返回值
			 *
			 * 检查OCI函数返回值，若有错误，抛出相应异常
			 *
			 * @param[in] status_ OCI函数返回值
			 */
			void CheckError(sword status_);

	};
}

#endif /* _INCLUDE_MDBBRIVER_ORACLE_H_ */
