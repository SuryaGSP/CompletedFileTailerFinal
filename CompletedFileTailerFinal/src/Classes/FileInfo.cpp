#include "FileInfo.h"
#include "DBOperations.h"
#include "ThreadSafeQueue.h"
#include "HelperFunctions.h"

std::vector<std::string> FileInfo::fileNamesMonitored;
std::vector<std::string> FileInfo::monitorDir;
std::map<std::string, std::vector<std::string>> FileInfo::dirIncludeTypeMap;
std::map<std::string, std::vector<std::string>> FileInfo::dirExcludeTypeMap;
std::map<std::string, std::string> FileInfo::dirPatternMap;
std::map<std::string, int> FileInfo::FileProcessMap;
std::map<std::string, std::string> FileInfo::FileRenameMap;
std::mutex FileInfo::m;

void FileInfo::pushRenameQueue(std::string fileName, std::string rename)
{
  ThreadSafeQueue<std::string> * modifiedQueueObject = getModifiedFileQueueObject();
  modifiedQueueObject->Push(fileName);
  std::unique_lock<std::mutex> lck(FileInfo::m);
  FileInfo::FileProcessMap[fileName] = 2;
  FileInfo::FileRenameMap[fileName] = rename;
}

void FileInfo::pushProcessQueue(std::string fileName)
{
  std::unique_lock<std::mutex> lck(FileInfo::m);
  if (FileProcessMap.find(fileName) != FileProcessMap.end())
  {
    if (FileProcessMap[fileName] != 1)
    {
      ThreadSafeQueue<std::string> * modifiedQueueObject = getModifiedFileQueueObject();
      modifiedQueueObject->Push(fileName);
      FileProcessMap[fileName] = 1;
    }
  }
}

void FileInfo::AddFileNamesMonitoredFirst(std::string fileName)
{
  fileNamesMonitored.push_back(fileName);
}
void FileInfo::AddFileNamesMonitored(std::string fileName)
{
  std::unique_lock<std::mutex> lck(FileInfo::m);
  FileProcessMap[fileName] = 0;
  ThreadSafeQueue<std::string> * modifiedQueueObject = getModifiedFileQueueObject();
  modifiedQueueObject->Push(fileName);
  FileProcessMap[fileName] = 1;
}

void FileInfo::PrintFileNamesMonitored()
{
  for (std::string &names : fileNamesMonitored)
  {
    FileInfo::AddFileNamesMonitored(names);
  }
}

bool FileInfo::FindDirInMonitorDir(std::string dirName)
{
  if (find(monitorDir.begin(), monitorDir.end(), dirName) == monitorDir.end())
    return true;
  return false;
}

void FileInfo::PushMonitorDirectory(std::string dirName)
{
  monitorDir.push_back(dirName);
}

void FileInfo::PrintMonitorDirectory()
{
  std::cout << "The directories to monitor are ";
  for (std::string & s : monitorDir)
  {
    std::cout << s << " ";
  }
  std::cout << std::endl;
}

int FileInfo::CountMonitorDirecs()
{
  return monitorDir.size();
}

std::vector<std::string> FileInfo::GetMonitorDirecsVector()
{
  return monitorDir;
}

void FileInfo::UpdateDB()
{
  for (std::string &s : fileNamesMonitored)
  {
    DBOperations::InsertQuery("insert or ignore into fileInfo values(?1,?2,?3)", s, 0, 0);
  }
}

void FileInfo::LoadDirectoryMap(std::string dirName, std::vector<std::string> includeTypesVector, std::vector<std::string> excludeTypesVector, std::string pattern)
{
  dirIncludeTypeMap[dirName] = includeTypesVector;
  dirExcludeTypeMap[dirName] = excludeTypesVector;
  dirPatternMap[dirName] = pattern;
}