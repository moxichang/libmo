/**********************************************************
 * Filename:    MDB.h
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

#ifndef _INCLUDE_MDB_H_
#define _INCLUDE_MDB_H_

#include <string>

#include "MDBDriver.h"
#include "MDBStatement.h"
#include "MDBResult.h"
#include "MDBRow.h"
#include "MDBColumn.h"
#include "MDBDate.h"

namespace MDB
{
	/*!
	 * @class MDBFactory
	 * @brief MDB工厂类
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-14
	 *
	 * 提供工厂模式，单例模式，数据源描述字符串（dsn）解析等操作
	 *
	 * @todo 
	 *    Singleton方法尚未实现
	 */
	class MDBFactory
	{
		public:
			/*!
			 * @brief 使用数据源描述字符串，创建相应 MDBDriver 对象
			 * 
			 * 工厂模式
			 * 
			 * @param[in] dsn_ 数据源描述字符串
			 *
			 * @return
			 *   -- MDBDriver 对象指针
			 */
			static MDBDriver* Factory(std::string dsn_);

			/*!
			 * @brief 使用数据源描述字符串，创建唯一的 MDBDriver 对象
			 * 
			 * 单例模式
			 * 多次调用都将返回指向同一对象的指针
			 * 
			 * @param[in] dsn_ 数据源描述字符串
			 *
			 * @return
			 *   -- MDBDriver 对象指针
			 */
			static MDBDriver* Singleton(std::string dsn_);

		private:

			/*!
			 * @brief 分析dsn字符串，解析出各个字段
			 *
			 * 分析dsn字符串，解析出各个字段
			 *
			 * @param[in] dsn_ 数据源描述字符串 
			 *
			 * @param[out] type_	数据库类型
			 * @param[out] user_name_ 数据库用户名
			 * @param[out] password_ 密码
			 * @param[out] host_ 数据库所在主机
			 * @param[out] port_ 端口号
			 * @param[out] database_ 数据库名
			 *
			 * @bug 
			 *	如果用户名或者密码中存在'@'等符号将会导致解析错误
			 *
			 * @todo
			 *	是否要添加Trim功能？
			 *
			 */
			static void ParseDSN(std::string dsn_,
					std::string& type_, std::string& user_name_, std::string& password_,
					std::string& host_, int& port_, std::string& database_);
			/*
			 * @brief 测试相应驱动是否存在
			 *
			 * @param[in] driver_name_ 驱动名
			 *
			 * @return 
			 *   -- true 驱动存在
			 *   -- false 驱动不存在
			 */
			static bool DriverAvailable(std::string driver_name_);

			//! 指向单例模式对象
			static MDBDriver *driver_singleton_m;

	};
}

#endif /* _INCLUDE_MDB_H_ */
