#pragma once
#include "ThreadSafeQueue.h"
void IntializeThreadPoolInFileProcessor();
ThreadSafeQueue<std::string>* getModifiedFileQueueObject();