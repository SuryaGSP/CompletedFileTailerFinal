#include <iostream>
#include "DBOperations.h"
#include "ThreadSafeQueue.h"
ThreadSafeQueue<std::string>* getModifiedFileQueueObject1();
void WorkerThreadForDB(int threadNo)
{
  int count = 0;
  ThreadSafeQueue<std::string> * modifiedInstance = getModifiedFileQueueObject1();
  while (true)
  {
    std::string str;
    if (!modifiedInstance->TryPop(str))
    {
      str = modifiedInstance->Pop();
    }
    DBOperations::UpdateRawQuery(str);
  }
}

ThreadSafeQueue<std::string>* getModifiedFileQueueObject1()
{
  static ThreadSafeQueue<std::string> * modifiedInstance;
  if (!modifiedInstance)
    modifiedInstance = new ThreadSafeQueue<std::string>();
  return modifiedInstance;
}