#include <errno.h>

#include "Exception.h"

#include "Miconv.h"

Miconv:: Miconv(const std::string& to_code_, const std::string& from_code_)
{
	cd_m = iconv_open( to_code_.c_str(), from_code_.c_str());

	if(cd_m == (iconv_t)(-1))
	{
		Throw("open iconv descriptor failed", errno);
	}
}

Miconv:: ~Miconv()
{
	if(iconv_close(cd_m))
	{
		Throw("close iconv descriptor failed", errno);
	}
}

size_t Miconv:: Convert(const std::string& in_, std::string& out_)
{
	size_t converted = 0;

	size_t inleft = in_.length() + 1;
	char *pin = (char *)in_.c_str();

	size_t outleft = in_.length() * 2;
	char *pout = new char[outleft + 1];

	if ( (converted = iconv(cd_m, &pin, &inleft, &pout, &outleft)) == (size_t) -1)
	{
		Throw("iconv return failed", errno);
	}

	out_ = pout;

	delete pout;

	return converted;
}

size_t Miconv:: Convert(const char *inbuf_, int inbytes_,
		const char *outbuf_, int outbytes_)
{
	size_t converted = 0;

	char *pin = (char *)inbuf_;
	char *pout = (char *)outbuf_;

	size_t inleft = inbytes_;
	size_t outleft = outbytes_;

	if ( (converted = iconv(cd_m, &pin, &inleft, &pout, &outleft)) == (size_t) -1)
	{
		Throw("iconv return failed", errno);
	}

	return converted;
}
