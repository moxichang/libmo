#ifndef _INCLUDE_MICONV_H_
#define _INCLUDE_MICONV_H_

#include <iconv.h>

#include <string>

class Miconv
{
	public:
		Miconv(const std::string& to_code_, const std::string& from_code_);
		~Miconv();

		size_t Convert(const std::string& in_, std::string& out_);
		size_t Convert(const char *inbuf_, int inbuf_size_,
				const char *outbuf_, int outbuf_size_);

	private:
		iconv_t cd_m;

};

#endif /* _INCLUDE_MICONV_H_ */
