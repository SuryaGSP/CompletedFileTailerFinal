#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <mutex>
class FileInfo
{
  static std::vector<std::string> fileNamesMonitored;
  static std::vector<std::string> monitorDir;
public:
  static std::mutex m;
  static std::map<std::string, int> FileProcessMap;
  static std::map<std::string, std::string> FileRenameMap;
  static std::map<std::string, std::vector<std::string>> dirIncludeTypeMap;
  static std::map<std::string, std::vector<std::string>> dirExcludeTypeMap;
  static std::map<std::string, std::string> dirPatternMap;

  static void pushRenameQueue(std::string fileName, std::string rename);
  static void pushProcessQueue(std::string fileName);
  static void AddFileNamesMonitored(std::string fileName);
  static void AddFileNamesMonitoredFirst(std::string fileName);
  static void PrintFileNamesMonitored();
  static bool FindDirInMonitorDir(std::string dirName);
  static void PushMonitorDirectory(std::string dirName);
  static void PrintMonitorDirectory();
  static int CountMonitorDirecs();
  static std::vector<std::string> GetMonitorDirecsVector();
  static void UpdateDB();
  static void LoadDirectoryMap(std::string dirName, std::vector<std::string> includeTypesVector, std::vector<std::string> excludeTypesVector, std::string pattern);
};