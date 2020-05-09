#ifndef MYSQLCONNECTIONPOOL_H_
#define MYSQLCONNECTIONPOOL_H_

#include <queue>
#include "ThreadLock.h"
#include "AgentConnection.h"

class MysqlConnectionPool {
public:
	/**
	* @nums : ��������
	* @url : ���ݿ����ӵ�ַ
	* @userName : �û���
	* @password : ����
	*/
	MysqlConnectionPool(const sql::SQLString& url, 
		const sql::SQLString& userName, const sql::SQLString& password, int nums = 1);
	// ԭ�ӵĻ�ȡһ������
	AgentConnection* getAgentConnection();
	// �������ӳ����������ӵ����ݿ�ʵ��
	void setSchema(const sql::SQLString& catalog);
	// Ŀǰ�ܵ�������
	int size() { return size_; }
private:
	// �ͷ����ӵ����ӳ�
	friend void AgentConnection::close();
	void freeConnection(AgentConnection* conn);
private:
	sql::Driver *driver_;
	// �ܵ���������
	std::list<AgentConnection*> connections_list_;
	// ���ж���
	std::queue<AgentConnection*> connection_free_queue_;
	// ���ж�����
	MyCriticalLock lock_;
	// ���ӳ���������
	int size_;
};


#endif //MYSQLCONNECTIONPOOL_H_
