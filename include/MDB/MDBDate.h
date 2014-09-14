/**********************************************************
 * Filename:    MDBDate.h
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

#ifndef _INCLUDE_MDBDATE_H_
#define _INCLUDE_MDBDATE_H_

#include <string>
#include <time.h>

namespace MDB
{
	/*!
	 * @class MDBDate
	 * @brief 数据库日期类型抽象类
	 * @author 莫锡昌
	 * @version 0.1
	 * @date 2009-05-3
	 *
	 * 日期类型抽象类，屏蔽不同数据库系统对日期类型定义的差异。
	 *
	 *
	 */
	class MDBDate
	{
		public:

			/*!
			 * @brief 默认构造函数
			 *
			 * 初始化对象为当前时间
			 */
			MDBDate();

			/*!
			 * @brief 构造函数
			 *
			 * 使用unix时间戳初始化对象
			 *
			 * @param[in] timestamp_ unix时间戳
			 */
			MDBDate(time_t timestamp_);

			/*!
			 * @brief 构造函数
			 *
			 * 以各个字段为参数初始化对象
			 *
			 * @param[in] year_ 年
			 * @param[in] month_ 月
			 * @param[in] day_ 日
			 * @param[in] hour_ 小时（24小时制）
			 * @param[in] minute_ 分钟
			 * @param[in] second_ 秒
			 */
			MDBDate(int year_, int month_, int day_, int hour_ = 0, int minute_ = 0, int second_ = 0);

			/*!
			 * @brief 返回年份
			 *
			 */
			int Year();

			/*!
			 * @brief 返回月份
			 *
			 */
			int Month();

			/*!
			 * @brief 返回月日期
			 *
			 */
			int Day();

			/*!
			 * @brief 返回星期中的某天
			 *
			 */
			int DayOfWeek();

			/*!
			 * @brief 返回一年中的某天
			 *
			 */
			int DayOfYear();

			/*!
			 * @brief 返回小时（24小时制）
			 *
			 */
			int Hour();

			/*!
			 * @brief 返回分钟
			 *
			 */
			int Minute();

			/*!
			 * @brief 返回秒数
			 *
			 */
			int Second();

			/*!
			 * @brief 格式化日期为字符串
			 *
			 * 返回由format_参数指定格式的日期字符串，和php的date函数类似。
			 * 
			 * 格式代码如下：
			 *
			 * y - 两位数表示的年份
			 * Y - 4位数完整表示年份
			 *
			 * m - 数字表示的年份，有前导零
			 * M - 数字表示的年份，无前导零
			 *
			 * d - 月份中的第几天，有前导零
			 * D - 月份中的第几天，无前导零
			 *
			 * g - 小时，12小时格式，无前导零
			 * G - 小时，24小时格式，无前导零
			 * h - 小时，12小时格式，有前导零
			 * H - 小时，24小时格式，有前导零
			 *
			 * i - 有前导零的分钟数
			 * I - 无前导零的分钟数
			 * 
			 * s - 有前导零的秒数
			 * S - 无前导零的秒数
			 *
			 * @param[in] format_ 格式字符串
			 *
			 * @return
			 *   -- 日期字符串
			 */
			std::string ToString(std::string format_="Y-m-d H:i:s");

			/*!
			 * @brief 获取unix时间戳
			 *
			 * @return
			 *   -- unix时间戳
			 */
			time_t Timestamp();

			/*!
			 * @brief 设置当前时区
			 *
			 * 默认为东八区（GMT+8）
			 *
			 * @param[in] timezone_ 时区
			 *
			 */
			void SetTimeZone(int timezone_);

			/*!
			 * @brief 获取当前时区
			 *
			 * @return
			 *   -- 当前时区
			 */
			int TimeZone();


		private:
			//! unix时间戳
			time_t timestamp_m;

			//! 时间结构体，存储各个时间字段
			struct tm times_m;

			//! 当前时区
			int timezone_m;

			/*!
			 * @brief 打印struct tm结构体
			 *
			 * @param[in] times struct tm结构体
			 */
			void PrintTime(struct tm *times);

	};
}

#endif /* _INCLUDE_MDBDATE_H_ */
