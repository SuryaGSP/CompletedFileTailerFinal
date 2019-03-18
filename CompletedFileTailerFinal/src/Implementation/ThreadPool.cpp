#define noOfThreads 4
#include <thread>
#include <string>
#include <map>
#include "ThreadSafeQueue.h"
#include "FileInfo.h"
#include "HelperFunctions.h"

void readRenamedFileData(std::string fileName, std::string renamedFileName);
void ReadFile(std::string fileName);
void WorkerThreads(int threadNo)
{
 ThreadSafeQueue<std::string> * modifiedInstance = getModifiedFileQueueObject();
  while (true)
  {
    std::string fileName;
    if (!modifiedInstance->TryPop(fileName))
    {
      fileName = modifiedInstance->Pop();
    }
    std::unique_lock<std::mutex> lck(FileInfo::m);
    if (FileInfo::FileProcessMap[fileName] == 2)
    {
      std::string rename = FileInfo::FileRenameMap[fileName];
      readRenamedFileData(fileName, rename);
      lck.unlock();
    }
    else if (FileInfo::FileProcessMap[fileName] == 1)
    {
      lck.unlock();
      ReadFile(fileName);
      std::unique_lock<std::mutex> lck(FileInfo::m);
      FileInfo::FileProcessMap[fileName] = 0;
    }
  }
}
void IntializeThreadPoolInFileProcessor() {
  std::thread t1[noOfThreads];
  for (int iter = 0; iter < noOfThreads; iter++)
  {
    t1[iter] = std::thread(WorkerThreads, iter);
  }
  for (std::thread &t : t1)
  {
    t.join();
  }
}