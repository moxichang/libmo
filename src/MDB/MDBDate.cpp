/**********************************************************
 * Filename:    MDBDate.cpp
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

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "Exception.h"
#include "MDB/MDBDate.h"

MDB::MDBDate:: MDBDate()
{
	timestamp_m = time(NULL);
	timezone_m = 8;

	if(localtime_r(&timestamp_m, &times_m) == NULL)
	{
		Throw("Transform date from timestamp to time struct failed.", MException::ME_INVARG);	
	}
}

MDB::MDBDate:: MDBDate(time_t timestamp_)
	: timestamp_m(timestamp_), timezone_m(8)
{
	if(localtime_r(&timestamp_, &times_m) == NULL)
	{
		Throw("Transform date from timestamp to time struct failed.", MException::ME_INVARG);	
	}
}

MDB::MDBDate:: MDBDate(int year_, int month_, int day_, int hour_, int minute_, int second_)
	:timezone_m(8)
{
	struct tm times;

	// FIXME
	//! @bug
	//! 1. can year be negative ?
	//! 2. Manual says: the second can be up to 60 to allow for leap seconds
	//!    what the fuck does this means?
	//!    see "man mktime"
	if(year_ < 0 || 
	   month_ < 1 || month_ > 12 ||
	   hour_ < 0 || hour_ > 23 ||
	   minute_ < 0 || minute_ > 59 ||
	   second_ < 0 || second_ > 59 ) 
	{
		Throw("Date format incorrect.",MException::ME_INVARG);
	}
	
	times.tm_year = year_ - 1900;
	times.tm_mon = month_ - 1;
	times.tm_mday = day_;
	times.tm_hour = hour_ ; // FIXME mktime need localtime
	times.tm_min = minute_;
	times.tm_sec = second_;

	if((timestamp_m = mktime(&times)) == -1)
	{
		Throw("Transform date to timestamp failed.", MException::ME_INVARG);
	}

	// 这样可以填充除了年月日时分秒之外的字段
	if(localtime_r(&timestamp_m, &times_m) == NULL)
	{
		Throw("Transform date from timestamp to time struct failed.", MException::ME_INVARG);	
	}
}

int MDB::MDBDate:: Year()
{

	return times_m.tm_year + 1900;
}

int MDB::MDBDate:: Month()
{

	return times_m.tm_mon + 1;
}

int MDB::MDBDate:: Day()
{

	return times_m.tm_mday;
}

int MDB::MDBDate:: DayOfWeek()
{
	return times_m.tm_wday;
}

int MDB::MDBDate:: DayOfYear()
{
	return times_m.tm_yday;
}

int MDB::MDBDate:: Hour()
{
	return times_m.tm_hour;
}

int MDB::MDBDate:: Minute()
{
	return times_m.tm_min;
}

int MDB::MDBDate:: Second()
{
	return times_m.tm_sec;
}

std::string MDB::MDBDate:: ToString(std::string format_)
{
	std::stringstream ss;

	for(unsigned int i=0; i< format_.length(); i++)
	{
		switch(format_[i])
		{
			case 'y':
				ss<<std::setfill('0')<<std::setw(2);
				ss<<Year()%100;
				break;

			case 'Y':
				ss<<Year();
				break;

			case 'm':
				ss<<std::setfill('0')<<std::setw(2);
				ss<<Month();
				break;

			case 'M':
				ss<<Month();
				break;

			case 'd':
				ss<<std::setfill('0')<<std::setw(2);
				ss<<Day();
				break;

			case 'D':
				ss<<Day();
				break;

			case 'g':
				ss<<(Hour()%12);
				break;

			case 'G':
				ss<<Hour();
				break;

			case 'h':
				ss<<std::setfill('0')<<std::setw(2);
				ss<<(Hour())%12;
				break;

			case 'H':
				ss<<std::setfill('0')<<std::setw(2);
				ss<<Hour();
				break;

			case 'i':
				ss<<std::setfill('0')<<std::setw(2);
				ss<<Minute();
				break;

			case 'I':
				ss<<Minute();
				break;

			case 's':
				ss<<std::setfill('0')<<std::setw(2);
				ss<<Second();
				break;

			case 'S':
				ss<<Second();
				break;

			default:
				ss<<format_[i];

		}
	}

	return ss.str();
}

time_t MDB::MDBDate:: Timestamp()
{
	return timestamp_m;
}

void MDB::MDBDate:: SetTimeZone(int timezone_)
{
	timezone_m = timezone_;
}

int MDB::MDBDate:: TimeZone()
{
	return timezone_m;
}

void MDB::MDBDate:: PrintTime(struct tm *times)
{

	std::cout<<"Year:\t"<<times->tm_year<<std::endl;
	std::cout<<"Month:\t"<<times->tm_mon<<std::endl;
	std::cout<<"Day:\t"<<times->tm_mday<<std::endl;

	std::cout<<"Hour:\t"<<times->tm_hour<<std::endl;
	std::cout<<"Minute:\t"<<times->tm_min<<std::endl;
	std::cout<<"Second:\t"<<times->tm_sec<<std::endl;

	std::cout<<"YDay\t"<<times->tm_yday<<std::endl;
	std::cout<<"WDay\t"<<times->tm_wday<<std::endl;

	//std::cout<<"\t"times->tm_isdst<<std::endl;

}



