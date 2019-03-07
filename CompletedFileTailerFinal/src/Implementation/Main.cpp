#include <iostream>
#include <string>
#include <thread>
#include "JSONProcessor.h"
#include "FileInfo.h"
#include "DirectoryMonitor.h"
#include "ThreadSafeQueue.h"
#include "DBOperations.h"
#include "HelperFunctions.h"
#include "ELALogger.h"

ELALogger * getLogObject()
{
  static ELALogger *logger = logger = new ELALogger();
  static int i = 0;
  if (i == 0)
  {
    logger->SetLogDir("D:/TestDir2");
    logger->SetExtension("txt");
    logger->SetLogFileSize(10000);
    logger->SetLogFileName("FileTailerLogs");
    logger->Start();
    i++;
  }
  return logger;
}

ThreadSafeQueue<std::string>* getModifiedFileQueueObject()
{
  static ThreadSafeQueue<std::string> * modifiedInstance;
  if (!modifiedInstance)
    modifiedInstance = new ThreadSafeQueue<std::string>();
  return modifiedInstance;
}

int main()
{
  DBOperations::OpenConnection("data");
  DBOperations::createTableIfNotExists("create table if not exists fileInfo(fileName text primary key , streampos INTEGER,skip INTEGER)");
  DBOperations::createTableIfNotExists("create table if not exists dirFilePatternInfo(dirName text primary key ,fileName text)");
  std::thread threadForThreadPool(IntializeThreadPoolInFileProcessor);
  std::string fileInput = "[{ \"locations\": [{ \"include\": \"D:\\\\TestDir\\\\\" }, { \"include\": \"D:\\\\TestDir2\\\\\", \"exclude\": [\"D:\\\\TestDir2\\\\archive\\\\\"], \"includetypes\": [\".log\"] }, { \"include\": \"D:\\\\App1\\\\\", \"exclude\": [\"D:\\\\App1\\\\*\"], \"excludetypes\": [\".dmp\", \".tmp\"] }]}]";// , { \"include\": \"D:\\\\logs\\\\\", \"pattern\": \"localhost_access_log.${dd}-${MM}-${yyyy}\" }, { \"include\": \"C:\\\\Users\\\\surya-pt2591\\\\Downloads\\\\apache-tomcat-8.5.37-windows-x64\\\\apache-tomcat-8.5.37\\\\logs\\\\\", \"pattern\": \"localhost_access_log.${yyyy}-${MM}-${dd}\" },{\"include\":\"D:\\\\Temp\\\\\",\"pattern\":\"out${NUMBER_1}\"}, { \"include\": \"D:\\\\LogDir\\\\logs\\\\\", \"pattern\": \"localhost_access_log.${MM}-${dd}-${yyyy}\" }] }]";
  JSONProcessor::ProcessJSON(fileInput);
  FileInfo::UpdateDB();
  FileInfo::PrintFileNamesMonitored();
  DirectoryMonitor::TotalDirectories = FileInfo::CountMonitorDirecs();
  DirectoryMonitor::LoadMonitorDirecs();
  std::vector<DirectoryMonitor> dirMoniObjects(DirectoryMonitor::TotalDirectories);
  int i = 0;
  for (std::string &s : DirectoryMonitor::monitorDirecs)
  {
    DirectoryMonitor *d = new DirectoryMonitor;
    d->SetDirName(s);
    d->Createhandle();
    d->SetOverlap();
    d->setNotifyBuffer();
    d->setDirDetails(FileInfo::dirIncludeTypeMap[s], FileInfo::dirExcludeTypeMap[s], FileInfo::dirPatternMap[s]);
    dirMoniObjects[i++] = (*d);
  }
  std::thread threadForMonitoring(DirectoryMonitor::CreateThreadForMonitoringDirectory, dirMoniObjects);
  std::cin.get();
}