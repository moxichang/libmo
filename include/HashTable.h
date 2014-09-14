/**********************************************************
 * Filename:    HashTable.h
 * Text Encoding: utf-8
 *
 * Description:
 *              
 *
 * Author: moxichang (ishego@gmail.com)
 *         Harbin Engineering University 
 *         Information Security Research Center
 *
 * Create Data:	2008-08-10
 * Last Update: 2009-04-01
 *
 *********************************************************/


#ifndef _INCLUDE_HASHTABLE_H_
#define _INCLUDE_HASHTABLE_H_

#include <string>

template < class Type >
class HashTable
{
	public:
		/*!
		 * @brief 构造函数
		 *
		 * 默认桶数为11，加载因子为0.75 \n
		 * 即当表中元素数与桶数之比大于0.75时，
		 * 表中的桶数加倍，重新哈希所有的元素。
		 * 
		 * @param[in] table_size_ 起始桶数 默认值11
		 * @param[in] load_factor_ 加载因子 默认值0.75
		 */
		HashTable(const size_t table_size_ = 11, float load_factor_ = 0.75);

		/*!
		 * @brief 拷贝构造函数
		 */
		HashTable(HashTable& ht_);

		/*!
		 * @brief 析构函数
		 */
		~HashTable();

		/*!
		 * @brief 将一对值放入哈希表
		 *
		 * 如果key_对应的单元在哈希表中已经有值
		 * 则使用当前的value_覆盖原来的value
		 * 
		 * @param[in] key_ 键名
		 * @param[in] value_ 值
		 */
		void Put(std::string key_, Type value_);

		/*!
		 * @brief 获取键值对应的值
		 *
		 * @param[in] key_ 键名
		 * @param[out] value_ 值
		 * 
		 * @return
		 * -- 0 成功找到值返回 true
		 * -- 1 未找到返回 false
		 */
		bool Get(std::string key_, Type& value_);

		/*!
		 * @brief 重载下标运算符
		 *
		 * 获取键index_对应的值
		 * 
		 * @param[in] index_ 键值
		 *
		 * @return
		 *	值的引用
		 */
		Type& operator [] (std::string index_);
		
		/*!
		 * @brief 重载赋值运算符
		 *
		 * 如果左边的对象已经初始化过，则清空
		 * 其中的元素。
		 *
		 * @param[in] ht_ 源哈希表
		 *
		 * @return
		 *	源哈希表的引用
		 */
		HashTable& operator = (HashTable& ht_);

		/*!
		 * @brief 移除key_对应的节点
		 *
		 * @param[in] key_ 键值
		 *
		 * @return
		 * -- true 成功找到并删除 
		 * -- false 未找到
		 */
		bool Remove(std::string key_);
		
		/*!
		 * @brief 清空哈希表，桶数保持不变
		 *
		 * @return
		 *	删除的节点数
		 */
		size_t Clear();

		/*!
		 * @brief 测试是否有键值对应的元素
		 *
		 * @return
		 * -- true 有
		 * -- false 无
		 */
		bool Contains(std::string key_);

		/*!
		 * @brief 测试哈希表是否为空
		 *
		 * @return
		 * -- true 有
		 * -- false 无
		 */
		bool IsEmpty();

		/*!
		 * @brief 返回哈希表中元素的个数
		 *
		 * @return
		 * 	哈希表中元素的个数
		 */
		int Size();

		/*!
		 * @brief 以树形方式打印整个哈希表
		 */
		void PrintAsTree();

		/*!
		 * @brief 以列表方式打印整个哈希表
		 */
		void PrintAsList();

	protected:
		/*!
		 * @brief 重新哈希整个表，将桶数扩大两倍
		 *
		 * 当哈希表中元素个数 key_number 超过定义的 rehash_size 时
		 * 调用此函数，扩大哈希表范围
		 */
		void Rehash();

	private:
		struct HashNode
		{
			std::string key;
			Type value;
			struct HashNode * next;

			HashNode(std::string key_, Type value_)
				: key(key_), value(value_), next(NULL)
			{
			}
		};

		unsigned int Hash(const char *key_, unsigned int key_len_);

		size_t table_size;
		size_t key_number;
		float load_factor;
		size_t rehash_size;

		HashNode **table;

		// FIXME!!! Hack for eliminate the complier waring
		static Type null_object; 

};

#include "../src/HashTable.tpl"

#endif /* _INCLUDE_HASHTABLE_H_ */

