#pragma once

#include <string>
#include <Windows.h>
#include <vector>
#include <map>
#include "FileInfo.h"
#include "ELALogger.h"

class DirectoryMonitor
{
  HANDLE handleDirectory;
  DWORD bytesReturned;
  OVERLAPPED o{ 0 };
  std::vector<std::string> includeTypes;
  std::vector<std::string> excludeTypes;
  std::string patternReference;
  FILE_NOTIFY_INFORMATION notificationBuffer[1024];
  FILE_NOTIFY_INFORMATION *notifyPointer;
public:
  std::string dirName;
  static int TotalDirectories;
  static ELALogger* logger;
  DirectoryMonitor();
  ~DirectoryMonitor();
  static std::vector<std::string> monitorDirecs;
  void SetDirName(std::string dirName);
  bool Createhandle();
  void SetOverlap();
  void setDirDetails(std::vector<std::string> includeTypes, std::vector<std::string> excludeTypes, std::string patternRef);
  void setNotifyBuffer();

  static void StartMonitoringDirectories(std::vector<DirectoryMonitor> &d);
  static bool refreshDirectory(DirectoryMonitor& d);
  static void CreateThreadForMonitoringDirectory(std::vector<DirectoryMonitor> d);
  static VOID CALLBACK CallBackFunction(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
  static void LoadMonitorDirecs();
};