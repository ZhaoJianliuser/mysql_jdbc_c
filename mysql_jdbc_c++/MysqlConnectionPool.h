#ifndef MYSQLCONNECTIONPOOL_H_
#define MYSQLCONNECTIONPOOL_H_

#include <queue>
#include "ThreadLock.h"
#include "AgentConnection.h"

class MysqlConnectionPool {
public:
	/**
	* @nums : 连接数量
	* @url : 数据库连接地址
	* @userName : 用户名
	* @password : 密码
	*/
	MysqlConnectionPool(const sql::SQLString& url, 
		const sql::SQLString& userName, const sql::SQLString& password, int nums = 1);
	// 原子的获取一个连接
	AgentConnection* getAgentConnection();
	// 设置连接池中所有连接的数据库实例
	void setSchema(const sql::SQLString& catalog);
	// 目前总的连接数
	int size() { return size_; }
private:
	// 释放连接到连接池
	friend void AgentConnection::close();
	void freeConnection(AgentConnection* conn);
private:
	sql::Driver *driver_;
	// 总的连接链表
	std::list<AgentConnection*> connections_list_;
	// 空闲队列
	std::queue<AgentConnection*> connection_free_queue_;
	// 空闲队列锁
	MyCriticalLock lock_;
	// 连接池总连接数
	int size_;
};


#endif //MYSQLCONNECTIONPOOL_H_
