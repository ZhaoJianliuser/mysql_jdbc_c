#include "MysqlConnectionPool.h"

MysqlConnectionPool::MysqlConnectionPool(const sql::SQLString& url, 
	const sql::SQLString& userName, const sql::SQLString& password, int nums) {

	size_ = 0;
	driver_ = nullptr;

	driver_ = get_driver_instance();
	for (int i = 0; i < nums; i++) {

		sql::Connection* conn = driver_->connect(url, userName, password);
		AgentConnection* agentConn = new AgentConnection(conn, this);
		connections_list_.push_back(agentConn);
		connection_free_queue_.push(agentConn);
		size_++;
	}
}

AgentConnection* MysqlConnectionPool::getAgentConnection() {

	AgentConnection* agentConnection = nullptr;
	lock_.lock();
	
	if (connection_free_queue_.empty()) {
		lock_.unlock();
		return nullptr;
	}

	agentConnection = connection_free_queue_.front();
	connection_free_queue_.pop();
	lock_.unlock();
	return agentConnection;
}

void MysqlConnectionPool::freeConnection(AgentConnection* conn) {

	lock_.lock();
	connection_free_queue_.push(conn);
	lock_.unlock();
}

void MysqlConnectionPool::setSchema(const sql::SQLString &catalog) {

	std::list<AgentConnection*>::iterator it = connections_list_.begin();
	for (; it != connections_list_.end(); it++) {
		(*it)->setSchema(catalog);
	}
}