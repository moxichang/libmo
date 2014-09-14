#include <iostream>
#include <stdexcept>
#include <iomanip>

#include "MDB/MDB.h"

using namespace std;
using namespace MDB;

int main(int argc, char **argv)
{
	MDBDriver *db = NULL;
	MDBResult *res = NULL;
	MDBRow *row = NULL;
	MDBStatement * stm;
	string sql;
	int affected;

	if(argc < 2)
	{
		cerr<<"usage: "<<argv[0]<<" DriverName"<<endl;
		return -1;
	}

	string dsn;
	dsn += argv[1];
	dsn += "://scott:tiger@localhost/ORCL";

	cout<<"DSN: "<<dsn<<endl;

	try{
		// 连接数据库
		//db = MDBFactory::Factory("oracle://scott:tiger@localhost/ORCL");
		//db = MDBFactory::Factory("mysql://scott:tiger@localhost/ORCL");

		getchar();

		db = MDBFactory::Factory(dsn);

		cout<<"Connect to database success."<<endl;
		cout<<endl;

		getchar();

		/////////////////////////////
		// 使用字符串形式的sql语句直接查询
		sql = "select * from emp";
		cout<<"Do query '"<<sql<<"':"<<endl;

		res = db->Query(sql);

		if(res == NULL)
		{
			cout<<"no data."<<endl;
		}

		cout<<"query success!"<<endl;
	
		// 打印输出结果
		int i=0;
		while( (row = res->FetchRow()) != NULL)
		{

			cout<<i<<"\t";

			// 使用下标数组的方式访问字段
			// 如果类型不正确，将抛出异常
			cout<<(*row)[0].AsInt()<<"\t";
			cout<<setfill(' ')<<setw(12);
			cout<<(*row)[1].AsString()<<"\t";
			cout<<setfill(' ')<<setw(10);
			cout<<(*row)[2].AsString()<<"\t";
			cout<<(*row)[3].AsInt()<<"\t";

			cout<<(*row)[4].AsDate()->ToString()<<"\t";
			//cout<<(*row)[4].AsDate()->ToString("y-M-D g-I-S")<<"\t";

			cout<<(*row)[5].AsFloat()<<"\t";
			cout<<(*row)[6].AsFloat()<<"\t";

			cout<<(*row)[7].AsInt()<<"\t";
			cout<<endl;
		
			i++;
		}

		cout<<endl;

		cout<<"Column number: "<<res->NumCols()<<endl;
		cout<<"Row number:    "<<res->NumRows()<<endl;

		cout<<endl;

		////////////////////////
		// 移动至结果集的第17行，行号从0开始
		res->Seek(8);

		cout<<"Seek row number to 8:"<<endl;

		// 重新打印结果
		i=0;
		while( (row = res->FetchRow()) != NULL)
		{

			cout<<i<<"\t";

			cout<<(*row)[0].AsInt()<<"\t";
			cout<<setfill(' ')<<setw(12);
			cout<<(*row)[1].AsString()<<"\t";
			cout<<setfill(' ')<<setw(10);
			cout<<(*row)[2].AsString()<<"\t";
			cout<<(*row)[3].AsInt()<<"\t";
			cout<<(*row)[4].AsDate()->ToString()<<"\t";

			cout<<(*row)[5].AsFloat()<<"\t";
			cout<<(*row)[6].AsFloat()<<"\t";

			cout<<(*row)[7].AsInt()<<"\t";
			cout<<endl;
		
			i++;
		}

		getchar();

		// 释放结果集
		delete res;
		cout<<endl;

		getchar();

		/////////////////////////////
		// 再查询一张不一样的表

		sql = "select * from dept";
		cout<<"Do query '"<<sql<<"':"<<endl;

		res = db->Query(sql);

		while( (row = res->FetchRow()) != NULL)
		{
			cout<<(*row)[0].AsInt()<<"\t";
			cout<<setfill(' ')<<setw(10);
			cout<<(*row)[1].AsString()<<"\t";
			cout<<(*row)[2].AsString()<<"\t";
			cout<<endl;
		}

		delete res;
		cout<<endl;

		///////////////////////////////////////////////////////////////////////////
		// 动态参数查询，按占位符名绑定
		sql = "select * from dept where deptno=:deptno";
		cout<<"Do query '"<<sql<<"':"<<endl;
	
		stm = db->Prepare(sql);

		int deptno = 20;

		// 用占位符变量绑定, int型
		stm->BindByName("deptno", NUMBER, &deptno, sizeof(int));

		res = db->Query(stm);

		while( (row = res->FetchRow()) != NULL)
		{
			cout<<(*row)[0].AsInt()<<"\t";
			cout<<setfill(' ')<<setw(10);
			cout<<(*row)[1].AsString()<<"\t";
			cout<<(*row)[2].AsString()<<"\t";
			cout<<endl;
		}

		delete res;
		// 释放查询句柄
		delete stm;
		cout<<endl;

		///////////////////////////////////////////////////////////////////////////
		sql = "select * from dept where dname=:dname";
		cout<<"Do query '"<<sql<<"':"<<endl;

		stm = db->Prepare(sql);

		char *dname = "SALES";

		// 用占位符变量绑定 字符串类型
		// 长度参数是要把'\0'包含进去的，否则执行时会出错
		stm->BindByName("dname", VARCHAR, dname, strlen(dname) +1 );

		res = db->Query(stm);

		while( (row = res->FetchRow()) != NULL)
		{
			cout<<(*row)[0].AsInt()<<"\t";
			cout<<setfill(' ')<<setw(10);
			cout<<(*row)[1].AsString()<<"\t";
			cout<<(*row)[2].AsString()<<"\t";
			cout<<endl;

		}

		delete res;
		delete stm;
		cout<<endl;

		///////////////////////////////////////////////////////////////////////////
		// 动态参数查询，按占位符位置绑定
		sql = "select * from dept where deptno=:1";
		cout<<"Do query '"<<sql<<"':"<<endl;

		stm = db->Prepare(sql);

		deptno = 30;

		stm->BindByPos(1, NUMBER, &deptno, sizeof(int));

		res = db->Query(stm);

		while( (row = res->FetchRow()) != NULL)
		{
			cout<<(*row)[0].AsInt()<<"\t";
			cout<<setfill(' ')<<setw(10);
			cout<<(*row)[1].AsString()<<"\t";
			cout<<(*row)[2].AsString()<<"\t";
			cout<<endl;

		}

		delete res;
		delete stm;
		cout<<endl;

		///////////////////////////////////////////////////////////////////////////
		// 执行非select查询
		sql = "update dept set loc = 'shanghai' where deptno=70 or deptno=50";
		cout<<"Do query '"<<sql<<"':"<<endl;

		// 注意使用Execute而不是Query
		// Execute返回查询影响的行数
		affected = db->Execute(sql);

		// affected为受影响的行数
		cout<<"Affected rows: "<<affected<<endl;

		// 提交事务
		db->Commit();
		cout<<endl;

		///////////////////////////////////////////////////////////////////////////
		// 使用函数
		sql =  "select max(empno) from emp";
		cout<<"Do query '"<<sql<<"':"<<endl;
		
		res = db->Query(sql);

		int max_empno = -1;

		if((row = res->FetchRow()) != NULL)
		{
			cout<<"Max empno:\t";
			cout<<(*row)[0].AsInt()<<endl;
			max_empno = (*row)[0].AsInt();
		}
		else
		{
			cout<<"fetch row failed."<<endl;
		}

		delete res;

		///////////////////////////////////////////////////////////////////////////
		// 插入操作，包括日期类型的处理
		sql = "insert into emp values(:1, :2, 'Proger', 123, :3, 10000, null, 10)";
		cout<<"Do query '"<<sql<<"':"<<endl;

		stm = db->Prepare(sql);

		int id = ++max_empno;
		const char *name = "farnsworth";
		MDBDate * date = new MDBDate();

		cout<<date->ToString()<<endl;

		stm->BindByPos(1, INTEGER, &id, sizeof(int) );
		stm->BindByPos(2, VARCHAR, name, strlen(name) + 1);
		stm->BindByPos(3, DATE, date, sizeof(MDBDate) );

		affected = db->Execute(stm);
		db->Commit();
		
		cout<<"Affected rows: "<<affected<<endl;

		delete date;
		delete stm;
		cout<<endl;


		//////////////////////////////////////////////////////////////////////////

		delete db;

		cout<<"Done."<<endl;
		getchar();

	}
	catch(invalid_argument& e)
	{
		cout<<"catch invalid argument exception: "<<e.what()<<endl;
	}
	catch(exception& e)
	{
		cout<<"other exception: "<<e.what()<<endl;
	}

	return 0;
}
