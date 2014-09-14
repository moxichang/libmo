#include <string.h>

#include <string>
#include <sstream>
#include <stdexcept>

#include "Exception.h"

void MException:: MyThrow(std::string message_, int err_code_, const char* filename_, const char* function_, int line_){

	char err_buf[ERR_LEN];
	std::stringstream info;


	if(err_code_ == -999)
	{
		info<<filename_<<"::"<<function_<<"::"<<line_<<" --> catch and throw.\n"<<message_;
	}
	else if(err_code_ <= 0 ){
		info<<filename_<<"::"<<function_<<"::"<<line_<<" --> "<<message_;
	}
	else{
		info<<filename_<<"::"<<function_<<"::"<<line_<<" --> "<<message_<<": "<<strerror_r(err_code_, err_buf, ERR_LEN);
	}

	switch(err_code_)
	{
		case ME_LOGIC:
			throw( std::logic_error(info.str()) );
			break;

		case ME_DOMAIN:
			throw( std::domain_error(info.str()) );
			break;

		case ME_INVARG:
			throw( std::invalid_argument(info.str()) );
			break;

		case ME_LENGTH:
			throw( std::length_error(info.str()) );
			break;

		case ME_OUTRANGE:
			throw( std::out_of_range(info.str()) );
			break;

		case ME_RUNTIME:
			throw( std::runtime_error(info.str()) );
			break;

		case ME_RANGE:
			throw( std::range_error(info.str()) );
			break;

		case ME_OVERFLOW:
			throw( std::overflow_error(info.str()) );
			break;

		case ME_UNDERFLOW:
			throw( std::underflow_error(info.str()) );
			break;

		case ME_RETHROW:
			throw( std::runtime_error(info.str()) );
			break;
		default:
			throw( sys_error(info.str()) );
	}

}
