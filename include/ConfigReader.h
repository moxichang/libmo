/**********************************************************
 * Filename:    ConfigReader.h
 *
 * Description:
 *              
 *
 * Author: moxichang (ishego@gmail.com)
 *         Harbin Engineering University 
 *         Information Security Research Center
 *
 * Create Data:	2008-10-20
 * Last Update: 2008-10-20
 *
 *********************************************************/

#ifndef _INCLUDE_CONFIG_H_
#define _INCLUDE_CONFIG_H_

#include <vector>
#include <string>

#include "HashTable.h"

/* 配置类 */
class ConfigReader{

	public:

		/* ---------------------------------- **
		** 默认构造函数
		**
		** ---------------------------------- */
		ConfigReader();

		/* ---------------------------------- **
		** 构造函数，从文件中读取配置
		** 参数：
		** 	config_file_: 要读取的配置文件
		** ---------------------------------- */
		ConfigReader(std::string config_file_);

		/* ---------------------------------- **
		** 从文件中读取配置
		** 参数：
		** 	config_file_: 要读取的配置文件
		** ---------------------------------- */
		void Load(std::string config_file_);

		/* ---------------------------------- **
		** 从文件中重新载入配置
		** ---------------------------------- */
		void Reload();

		/* ---------------------------------- **
		** 重载[]运算符，使得可以通过键值访问
		** 配置值，如通过configs["Google"]访问
		** 配置文件中
		** 	Google www.google.cn
		** 这一条配置
		** 参数：
		** 	key_: 键名
		** ---------------------------------- */
		std::string& operator [] ( std::string key_ );

		/* ---------------------------------- **
		** 添加一个配置
		** 参数：
		**	key_：键名
		**	value_：值
		** ---------------------------------- */
		void Add(std::string key_, std::string value_);

		/* ---------------------------------- **
		** 过滤函数，去掉字符串首尾的空白字符以及
		** 注释掉的内容。
		** 默认去除的字包括：空格，\r，\n，\t
		** 也可以通过charlist_参数自行指定要删除
		** 的字符
		** ---------------------------------- */
		std::string Trim(std::string &str, std::string charlist_ = "\n\t\r ");

		/* ---------------------------------- **
		** 小工具，打印现有配置
		** ---------------------------------- */
		void PrintAll();

	protected:
		// 哈希表，存储数据用
		HashTable <std::string> config_table_m;
		
		// 当前配置文件名
		std::string config_file_m;

};

#endif /* _INCLUDE_CONFIG_H_ */
