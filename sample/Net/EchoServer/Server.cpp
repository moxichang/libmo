#include <iostream>

#include <signal.h>

#include <Exception.h>
#include <Net/TcpListener.h>

using namespace std;

#define BUF_SIZE 32
#define TIMEOUT 5000000

int main()
{
	// 避免出现僵尸进程
	signal(SIGCHLD, SIG_IGN);

	// 创建TcpListener对象
	// 监听来自9090端口的连接
	TcpListener *tl = new TcpListener(9090);

	pid_t ch_id;

	while(1)
	{
		// 新连接到来
		TcpClient *tc = tl->Accept();

		cout<<"accept new connection form "<<tc->GetRemoteAddress()<<":"<<tc->GetRemotePort()<<endl;

		if ((ch_id = fork()) == 0)
		{
			delete tl;
			tl = NULL;

			char *buf;
			int bytes;

			buf = new char[BUF_SIZE + 1];

			try
			{
				// 接收数据
				// Recv正常返回接收到的字节数
				// 当对方关闭连接时返回0，其他情况抛出异常
				while( (bytes = tc->Recv(buf, BUF_SIZE, TIMEOUT) ))
				{
					buf[bytes] = '\0';
					// 打印数据
					cout<<bytes<<": "<<buf;
				}
				cout << endl;
			}
			catch(exception &e)
			{
				cout<<"\ncatch exception: "<<e.what()<<endl;
			}

			// 删除TcpClient对象
			delete tc;
			delete [] buf;

			cout<<"lost connection with client."<<endl;

			exit(0);
		}
	
		delete tc;
		tc = NULL;

	}

	delete tl;

	return 0;
}
