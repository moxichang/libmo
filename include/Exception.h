#ifndef _INCLUDE_EXCEPTION_H_
#define _INCLUDE_EXCEPTION_H_

#include <string>
#include <stdexcept>

#include <errno.h>

#define Throw(message_, err_code_) \
		MException::MyThrow(message_, err_code_, __FILE__, __FUNCTION__, __LINE__)

/*!
 * @class sys_error
 * @brief 系统异常类
 * @author 莫锡昌
 * @version 0.1
 * @date 2009-04-22
 *
 * 继承自标准异常类 runtime_error 
 * 用来表示Linux系统错误
 *
 */
class sys_error : public std::runtime_error
{
	public:
		sys_error(const std::string& msg = "")
			 : std::runtime_error(msg)
		{}
};

namespace MException {

	// FIXME
	//! @bug errno有可能不为0（用户自定义函数可改变errno的值）
	const int ME_LOGIC = -1;
	const int ME_DOMAIN = -2;
	const int ME_INVARG = -3;
	const int ME_LENGTH = -4;
	const int ME_OUTRANGE = -5;

	const int ME_RUNTIME = -6;
	const int ME_RANGE = -7;
	const int ME_OVERFLOW = -8;
	const int ME_UNDERFLOW = -9;

	const int ME_RETHROW = -999;

	const int ERR_LEN = 200;

	/*!
	 * @brief 异常抛出函数
	 * 
	 * 接收错误代码，文本描述信息，文件名等作为参数 
	 * 根据代码类型，选择抛出不同类型的异常。
	 * \n
	 * 当 err_code_ 为 linux中的errno时，获取对应的错误描述信息，
	 * 抛出自定义的sys_error异常
	 *
	 * @param[in] message_ 用户给出的错误描述
	 * @param[in] err_code_ 错误代码或错误类型
	 * @param[in] filename_ 抛出异常的文件
	 * @param[in] function_ 抛出异常的函数
	 * @param[in] line_ 抛出异常的代码行
	 *
	 */
	void MyThrow(std::string message_, int err_code_, const char* filename_, const char* function_, int line_);

}

#endif /* _INCLUDE_EXCEPTION_H_ */
