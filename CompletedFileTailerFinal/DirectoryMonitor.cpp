#include <regex>
#include "DirectoryMonitor.h"
#include "DBOperations.h"
#include "ELALogger.h"
#include "ThreadSafeQueue.h"
#include "HelperFunctions.h"

ELALogger* getLogObject();
int DirectoryMonitor::TotalDirectories;
std::vector<std::string> DirectoryMonitor::monitorDirecs;
ELALogger* DirectoryMonitor::logger = NULL;
void AddFileMonitor(std::string fileName)
{
  DBOperations::InsertQuery("INSERT INTO fileInfo VALUES(?1, ?2 ,?3)  ON CONFLICT(fileName) DO UPDATE SET streampos = ?2 , skip = ?3", fileName, 0, 0);
  FileInfo::AddFileNamesMonitored(fileName);
}

DirectoryMonitor::DirectoryMonitor()
{
}

DirectoryMonitor::~DirectoryMonitor()
{
}
void DirectoryMonitor::SetDirName(std::string dirName)
{
  this->dirName = dirName;
  return;
}

bool DirectoryMonitor::Createhandle()
{
  this->handleDirectory = CreateFile(this->dirName.c_str(), GENERIC_READ | GENERIC_WRITE | FILE_LIST_DIRECTORY, FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
  if (!this->handleDirectory)
  {
    return false;
  }
  return true;
}

void DirectoryMonitor::SetOverlap()
{
  this->o.hEvent = this;
  return;
}

void DirectoryMonitor::StartMonitoringDirectories(std::vector<DirectoryMonitor> &d)
{
  for (DirectoryMonitor &directory : d)
  {
    if (refreshDirectory(directory))
    {
      logger->info(" Monitoring Directory %v", directory.dirName);
    }
    else
    {
      logger->info("stopped Monitoring Directory %v", directory.dirName);
    }
  }
  while (true)
  {
    SleepEx(INFINITE, true);
  }
}

bool DirectoryMonitor::refreshDirectory(DirectoryMonitor &d)
{
  return ReadDirectoryChangesW((&d)->handleDirectory, (&d)->notifyPointer, sizeof((&d)->notificationBuffer), false, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME, &(&d)->bytesReturned, &(&d)->o, &CallBackFunction);
}

void DirectoryMonitor::CreateThreadForMonitoringDirectory(std::vector<DirectoryMonitor> d)
{
  logger = getLogObject();
  StartMonitoringDirectories(d);
}

VOID DirectoryMonitor::CallBackFunction(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
  FILE_NOTIFY_INFORMATION *fni;
  DirectoryMonitor * direcMoniObject = (DirectoryMonitor *)(lpOverlapped->hEvent);
  if (dwErrorCode == ERROR_SUCCESS)
  {
    size_t notificationBufferCount = 0;
    fni = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(&direcMoniObject->notificationBuffer[notificationBufferCount]);
    int fileNameLength = fni->FileNameLength / sizeof(WCHAR);
    if (fileNameLength > 0)
    {
      fni->FileName[fni->FileNameLength / sizeof(WCHAR)] = '\0';
      std::string fileName = LoggerUtil::WideCharToMultiByte(fni->FileName);
      if (fni->Action == FILE_ACTION_ADDED)
      {
        logger->info("File Created %v", fileName);
        if (direcMoniObject->includeTypes.size() == 0 && direcMoniObject->excludeTypes.size() == 0)
        {
          if (direcMoniObject->patternReference != "()")
          {
            std::string fileNameWithoutExt = LoggerUtil::GetFileNameWithOutExtension(fileName);
            std::smatch m;
            std::regex regInstance(direcMoniObject->patternReference);
            if (std::regex_match(fileNameWithoutExt, m, regInstance))
            {
              DBOperations::UpdateQuery("update dirFilePatternInfo set fileName = ?1 where dirName = ?2", fileName, direcMoniObject->dirName);
              AddFileMonitor(direcMoniObject->dirName + "" + fileName);
            }
          }
          else
          {
            AddFileMonitor(direcMoniObject->dirName + "" + fileName);
          }
        }
        else
        {
          bool include = false;
          if (direcMoniObject->includeTypes.size() != 0)
          {
            if (find(direcMoniObject->includeTypes.begin(), direcMoniObject->includeTypes.end(), "." + LoggerUtil::GetExtension(fileName)) != direcMoniObject->includeTypes.end())
            {
              if (direcMoniObject->patternReference != "()")
              {
                std::string fileNameWithoutExt = LoggerUtil::GetFileNameWithOutExtension(fileName);
                std::smatch m;
                std::regex regInstance(direcMoniObject->patternReference);
                if (std::regex_match(fileNameWithoutExt, m, regInstance))
                {
                  DBOperations::UpdateQuery("update dirFilePatternInfo set fileName = ?1 where dirName = ?2", fileName, direcMoniObject->dirName);
                  AddFileMonitor(direcMoniObject->dirName + "" + fileName);
                }
              }
              else
              {
                AddFileMonitor(direcMoniObject->dirName + "" + fileName);
              }
            }
          }
          else
          {
            if (find(direcMoniObject->excludeTypes.begin(), direcMoniObject->excludeTypes.end(), "." + LoggerUtil::GetExtension(fileName)) == direcMoniObject->excludeTypes.end())
            {
              if (direcMoniObject->patternReference != "()")
              {
                std::string fileNameWithoutExt = LoggerUtil::GetFileNameWithOutExtension(fileName);
                std::smatch m;
                std::regex regInstance(direcMoniObject->patternReference);
                if (std::regex_match(fileNameWithoutExt, m, regInstance))
                {
                  DBOperations::UpdateQuery("update dirFilePatternInfo set fileName = ?1 where dirName = ?2", fileName, direcMoniObject->dirName);
                  AddFileMonitor(direcMoniObject->dirName + "" + fileName);
                }
              }
              else
              {
                AddFileMonitor(direcMoniObject->dirName + "" + fileName);
              }
            }
          }
        }
      }
      else if (direcMoniObject->notificationBuffer->Action == FILE_ACTION_RENAMED_OLD_NAME || direcMoniObject->notificationBuffer->Action == FILE_ACTION_RENAMED_NEW_NAME)
      {
        FILE_NOTIFY_INFORMATION * fni1 = (FILE_NOTIFY_INFORMATION *)((char *)direcMoniObject->notificationBuffer + direcMoniObject->notificationBuffer->NextEntryOffset);
        std::string rfileName = LoggerUtil::WideCharToMultiByte(fni1->FileName);
        logger->info("File Renamed from %v  to %v", fileName, rfileName);
        FileInfo::pushRenameQueue(direcMoniObject->dirName + "" + fileName, direcMoniObject->dirName + "" + rfileName);
      }
      else if (direcMoniObject->notificationBuffer->Action == FILE_ACTION_MODIFIED)
      {
        FileInfo::pushProcessQueue(direcMoniObject->dirName + "" + fileName);
      }
      else if (direcMoniObject->notificationBuffer->Action == FILE_ACTION_REMOVED)
      {
        logger->info("File Removed%v", "");
      }
    }
    if (!refreshDirectory(*direcMoniObject))
    {
      logger->info("Monitoring Failed %v", direcMoniObject->dirName);
    }
  }
  else if (dwErrorCode == ERROR_OPERATION_ABORTED)
  {
    logger->info("Operation aborted in %v", direcMoniObject->dirName);
    CloseHandle(direcMoniObject->handleDirectory);
    return;
  }
  else
  {
    logger->info(" unknown error:: Error Code %v", dwErrorCode);
  }
  return;
}

void DirectoryMonitor::LoadMonitorDirecs()
{
  monitorDirecs = FileInfo::GetMonitorDirecsVector();
}

void DirectoryMonitor::setDirDetails(std::vector<std::string> includeTypes, std::vector<std::string> excludeTypes, std::string patternRef)
{
  this->includeTypes = includeTypes;
  this->excludeTypes = excludeTypes;
  this->patternReference = patternRef;
}

void DirectoryMonitor::setNotifyBuffer()
{
  this->notifyPointer = notificationBuffer;
}

