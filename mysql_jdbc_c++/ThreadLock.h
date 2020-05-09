#ifndef MYTHREADLOCK_H_
#define MYTHREADLOCK_H_

#include <Windows.h>

class MyCriticalLock {
public:
	MyCriticalLock();
	~MyCriticalLock();

	void lock();
	void unlock();
private:
	CRITICAL_SECTION critical_section_;
};


#endif //MYTHREADLOCK_H_
