/**********************************************************
 * Filename:    ConfigReader.cpp
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

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "ConfigReader.h"

#define MAX_LINE_SIZE 256

ConfigReader::ConfigReader()
	: config_file_m("")
{

}

ConfigReader::ConfigReader(std::string config_file_)
	: config_file_m(config_file_)
{
	Load(config_file_);
}

void ConfigReader::Load(std::string config_file_)
{
	// 文件名不为空，说明不是初次次载入
	// 需要清空哈希表
	if( ! config_file_m.empty() ){
		config_table_m.Clear();
	}

	// FIXME!!! 是否需要检查文件是否存在 ?
	this->config_file_m = config_file_;

	char raw_line[MAX_LINE_SIZE];
	std::ifstream ifs(config_file_.c_str());

	if(ifs.fail()){
		std::stringstream msg;
		msg<<"open config file '"<<config_file_<<"' failed";
		Throw(msg.str(),0);
	}

	while( ifs.eof() == false ){
		std::stringstream sformat;
		std::string key,value;
		std::string line;

		ifs.getline(raw_line, MAX_LINE_SIZE);
		line = raw_line;

		Trim(line);

		if(line.size() == 0){
			continue;
		}

		sformat<<line;
		sformat>>key>>value;

		config_table_m.Put(key,value);
	}

	ifs.close();
}

void ConfigReader::Reload()
{
	if(config_file_m.empty()){
		Throw("config file name unspecified",0);
	}

	char raw_line[MAX_LINE_SIZE];
	std::ifstream ifs(config_file_m.c_str());

	if(ifs.fail()){
		std::stringstream msg;
		msg<<"open config file '"<<config_file_m<<"' failed";
		Throw(msg.str(),0);
	}

	while( ifs.eof() == false ){
		std::stringstream sformat;
		std::string key,value;
		std::string line;

		ifs.getline(raw_line, MAX_LINE_SIZE);
		line = raw_line;

		Trim(line);

		if(line.size() == 0){
			continue;
		}

		sformat<<line;
		sformat>>key>>value;

		// FIXME !!!
		// 需要删除已经不存在的值吗？
		config_table_m.Put(key,value);
	}

	ifs.close();
}


std::string ConfigReader::Trim(std::string &str_, std::string charlist_)
{

	std::string::size_type pos = str_.find_first_of('#');
	if(pos != std::string::npos){
		str_.erase(pos);
	}

	if(str_.size() == 0)
		return str_;

	for(size_t i=0; i< charlist_.length(); i++){

		char t = charlist_[i];

		pos = str_.find_last_not_of(t);
		if(pos != std::string::npos) {
			str_.erase(pos + 1);
			pos = str_.find_first_not_of(t);
			if(pos != std::string::npos)
				str_.erase(0, pos);
		}
		else{
			str_.erase(str_.begin(), str_.end());
		}

		if(str_.size() == 0)
			break;
	}

	return str_;
}

std::string& ConfigReader::operator [] ( std::string key_ )
{

	return config_table_m[key_];
}

void ConfigReader::Add(std::string key_, std::string value_)
{
	config_table_m.Put(key_, value_);	
}

void ConfigReader::PrintAll()
{
	config_table_m.PrintAsList();
}

