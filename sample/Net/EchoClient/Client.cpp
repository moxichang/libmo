#include <iostream>

#include <signal.h>

#include <Exception.h>
#include <Net/TcpListener.h>

using namespace std;

int main()
{
	string ip = "127.0.0.1";
	int port = 9090;
	
	TcpClient *tc = NULL;
	
	try
	{
		tc = new TcpClient(ip, port);
		cout << "Connect " << ip << ":" << port << " success" << endl;
		
		tc->CSend("this is a test", strlen("this is a test"));
		cout << "Send data OK" << endl;
		
		delete tc;
		tc = NULL;
	}
	catch(exception &e)
	{
		cout << "catch exception" << endl;
		cout << e.what() << endl;
		delete tc;
		tc = NULL;
	}
	

	return 0;
}


