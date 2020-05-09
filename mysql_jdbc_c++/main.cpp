#include <iostream>
#include <string>
#include <stdint.h>
#include <time.h>
#include <thread>
#include <regex>


#include "WinTimer.h"
#include "mysql/jdbc.h"
#include "MysqlConnectionPool.h"

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )


using namespace std;
using namespace sql;

typedef unsigned long ulong;

#define NUMOFFSET 100
#define COLNAME 200

// 请求总数
volatile long requests_count_ = 0;

string ip_generator() {

	//Sleep(1000);

	string ipout;
	// 设置随机种子
	//srand((int)GetTickCount());
	
	int part_1 = rand() % 238 + 1;
	int part_2 = rand() % 255;
	int part_3 = rand() % 255;
	int part_4 = rand() % 255;

	ipout += to_string(part_1);
	ipout += ".";
	ipout += to_string(part_2);
	ipout += ".";
	ipout += to_string(part_3);
	ipout += ".";
	ipout += to_string(part_4);

	return ipout;
}

static void retrieve_data_and_print(ResultSet* rs, int type, int colidx, string colname) {

        /* retrieve the row count in the result set */
        //cout << "\nRetrieved " << rs->rowsCount() << " row(s)." << endl;
	if (rs->rowsCount() < 1) {
		cout << "the outcome is zero" << endl;
		return;
	}

        /* fetch the data : retrieve all the rows in the result set */
        while (rs->next()) {
                if (type == NUMOFFSET) {
			SQLString s =  rs->getString(colidx);
                        cout << s.c_str() << " ";
                }
                else if (type == COLNAME) {

                        cout << rs->getString(colname.c_str()).c_str() << " ";
                } // if-else
        } // while

        cout << endl;

} // retrieve_data_and_print()


static void retrieve_rsmetadata_and_print(ResultSet* rs) {

	if (rs->rowsCount() == 0) {
		throw runtime_error("ResultSetMetaData FAILURE - no records in the result set");
	}

	cout << "ResultSet Metadata" << endl;
	cout << "------------------" << endl;

	/* The following commented statement won't work with Connector/C++ 1.0.5 and later */
	//auto_ptr < ResultSetMetaData > res_meta ( rs -> getMetaData() );

	ResultSetMetaData* res_meta = rs->getMetaData();

	int numcols = res_meta->getColumnCount();
	cout << "\nNumber of columns in the result set = " << numcols << endl << endl;

	cout.width(20);
	cout << "Column Name/Label";
	cout.width(20);
	cout << "Column Type";
	cout.width(20);
	cout << "Column Size" << endl;

	for (int i = 0; i < numcols; ++i) {
		cout.width(20);
		cout << res_meta->getColumnLabel(i + 1);
		cout.width(20);
		cout << res_meta->getColumnTypeName(i + 1);
		cout.width(20);
		cout << res_meta->getColumnDisplaySize(i + 1) << endl << endl;
	}

	cout << "\nColumn \"" << res_meta->getColumnLabel(1);
	cout << "\" belongs to the Table: \"" << res_meta->getTableName(1);
	cout << "\" which belongs to the Schema: \"" << res_meta->getSchemaName(1) << "\"" << endl << endl;

} // retrieve_rsmetadata_and_print()

//字符串分割函数
std::vector<std::string> split(std::string str, const std::string &pattern)
{
	int pos = 0;
	std::vector<std::string> result;

	if (str.empty() || pattern.empty())
		return result;

	str += pattern;
	for (int i = 0; i < str.size();)
	{
		pos = str.find(pattern, i);

		if (pos < str.size())
		{
			result.push_back(str.substr(i, pos - i));
			i = pos + pattern.size();
			continue;
		}
		break;
	}
	return result;
}

ulong long2ip(const std::string &ip)
{
	if (ip.empty())
		return 0;

	ulong iplong = 0;
	std::vector<std::string> partList = split(ip, "."); // 这里还是需要对ip做校验

	iplong += atoi(partList[0].c_str()) << 24;
	iplong += atoi(partList[1].c_str()) << 16;
	iplong += atoi(partList[2].c_str()) << 8;
	iplong += atoi(partList[3].c_str());

	return iplong;
}

void worker_thread(void* lparam) {

	srand((int)GetTickCount());

	MysqlConnectionPool* pool = (MysqlConnectionPool*)lparam;

	AgentConnection* conn = nullptr;
	Statement* stmt = nullptr;

	while (1) {

		conn = pool->getAgentConnection();

		if (nullptr == conn) {

			Sleep(100);
			continue;
		}

		string newip = ip_generator();
		ulong iplong = long2ip(newip);

		std::string ipLocationSql;
		ipLocationSql += "SELECT * from ip_data where ip1 <= ";
		ipLocationSql += to_string(iplong);
		ipLocationSql += " AND ";
		ipLocationSql += to_string(iplong);
		ipLocationSql += " <= ip2";

		MyTimer timer;
		ResultSet* res = nullptr;
		stmt = conn->createStatement();

		timer.start();
		res = stmt->executeQuery(ipLocationSql.c_str());
		timer.stop();
		double intervals = timer.elapse();

		res->next();
		string ss = res->getString("province").c_str();
		// 增加请求计数
		InterlockedAdd(&requests_count_, 1);

		string debugInfo;
		debugInfo += "MYSQL TEST ===== province : ";
		debugInfo += ss;
		debugInfo += " ======= request counts : ";
		debugInfo += to_string(requests_count_);

		OutputDebugString(debugInfo.c_str());
		OutputDebugString("\n");

		conn->close();
		conn = nullptr;

		if(nullptr != stmt)
			delete stmt;
		stmt = nullptr;

		if (nullptr != res)
			delete res;
		res = nullptr;	
	}
}

int main(int argc, char* argv[])
{
	// 校验IP地址的正则表达式
	//string goal("11.0.4.6");
	//regex re("(([0-9]|([1-9][0-9])|([1-2][0-4][0-9])|([1-2]5[0-5])).){3}([0-9]|([1-9][0-9])|([1-2][0-4][0-9])|([1-2]5[0-5]))");

	//assert(regex_match("255.255.255.255", re));
	//assert(regex_match("0.0.0.0", re));
	//assert(regex_match("127.0.1.0", re));
	//assert(!regex_match("123.45.679.2", re));
	//assert(!regex_match("111.456.112.213", re));
	//assert(!regex_match("9.01.3.12", re));


	regex re("(([0-9]|([1-9][0-9])|([1-2][0-4][0-9])|([1-2]5[0-5])).){3}([0-9]|([1-9][0-9])|([1-2][0-4][0-9])|([1-2]5[0-5]))");

	assert(regex_match("255.255.255.255", re));
	assert(regex_match("0.0.0.0", re));
	assert(regex_match("127.0.1.0", re));
	assert(regex_match("2551.255.255.255", re));
	assert(!regex_match("111.456.112.213", re));
	assert(!regex_match("9.01.3.12", re));

	
	AgentConnection* conn = nullptr;
	Statement* stmt = nullptr;

	MysqlConnectionPool* pool = new MysqlConnectionPool("192.168.213.131:3306", "root", "083415", 10);
	pool->setSchema("tcpcc");

	// 创建多个执行线程
	const int thread_numbers = 16;
	std::thread th[thread_numbers];

	for (int i = 0; i < thread_numbers; i++) {

		Sleep(1000);
		th[i] = thread(worker_thread, pool);
	} 

	for (int j = 0; j < thread_numbers; j++)
		th[j].join();

	return 0;
}