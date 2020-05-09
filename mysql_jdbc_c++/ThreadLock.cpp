#include "ThreadLock.h"

MyCriticalLock::MyCriticalLock() {
	::InitializeCriticalSection(&critical_section_);
}

MyCriticalLock::~MyCriticalLock() {
	::DeleteCriticalSection(&critical_section_);
}

void MyCriticalLock::lock() {
	::EnterCriticalSection(&critical_section_);
}

void MyCriticalLock::unlock() {
	::LeaveCriticalSection(&critical_section_);
}