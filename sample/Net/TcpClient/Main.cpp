#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>

#include <netdb.h>

#include <Net/TcpClient.h>

using namespace std;

int main()
{

	string hostname = "acm.hrbeu.edu.cn";
	string path = "/";
	char cip[50];

	struct hostent * host;
	if((host = gethostbyname(hostname.c_str())) == NULL)
	{
		cout << "gethostbyname failed" << endl;
		return -1;
	}
	memcpy(cip, host->h_addr, host->h_length);
	string ip (cip);
	cout << "ip = " << ip << endl;
	
	try
	{
		TcpClient *tcl = new TcpClient(ip, 80);

		cout<<"connect to "<<hostname<<" success."<<endl;

		stringstream request;

		request<<"GET "<<path<<" HTTP/1.0\r\n"
			"Host: "<<hostname<<"\r\n"
			"User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.0.6) Gecko/2009011913 Firefox/3.0.6\r\n"
			"Accept: text/html\r\n"
			"Accept-Language: zh,zh-cn\r\n"
			"Accept-Encoding: deflate\r\n"
			"Accept-Charset: gb2312,utf-8;\r\n"
			"Connection: close\r\n"
			"\r\n";

		tcl->CSend(request.str().c_str(), request.str().length());

		cout<<"-----------------Request Send Success.-------------------"<<endl;
		cout<<request.str()<<endl;
		cout<<"---------------------------------------------------------"<<endl;

		stringstream response;
		int body_length = 0;

		bool head_down = false;

		while(true)
		{
			string line;
			int retval = 0;

			do{

				retval = tcl->RecvCLRF(line, 5000000);

				response<<line<<endl;

				if((line.find("Content-Length")) != string::npos)
				{
					size_t pos;
					if( (pos= line.find(":")) == string::npos)
					{
						cout<<"shit happens!"<<endl;
					}

					line.erase(0, pos+1);
					stringstream ss(line);
					ss>>body_length;
					
				}

				if(line.compare("") == 0)
				{
					head_down = true;
					break;
				}

			}while(1);

			if(head_down)
			{
				break;
			}
		}

		cout<<"-----------------------Response--------------------------"<<endl;
		cout<<response.str();

		string body;

		// HTTP头中有Content-Length字段
		//if(body_length != 0)
		if(false)
		{
			char *buf = new char[body_length + 1];

			// 使用CRecv收取定长数据
			tcl->CRecv(buf, body_length);
			buf[body_length] = '\0';

			body = buf;

			delete buf;
		}
		else
		{
			int bytes;
			char *buf = new char[33];

			do
			{
				// 使用Recv收取不定长数据
				// 当Recv返回值为0时，说明连接已经断开
				bytes = tcl->Recv(buf, 32);
				buf[bytes] = '\0';

				body += buf;
			}
			while(bytes != 0);

			delete buf;

		}

		cout<<body<<endl;

		cout<<"---------------------------------------------------------"<<endl;

	}
	catch(exception &e)
	{
		cout<<"catch exception:"<<endl;
		cout<<e.what()<<endl;
	}

	return 0;
}
