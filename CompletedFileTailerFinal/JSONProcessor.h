#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <Windows.h>
#include "ELALogger.h"
#include "SystemUtil.h"
#include <regex>
#include "rapidjson/document.h"
#include "JSONProcessor.h"
class JSONProcessor
{
  static std::string patternForDirectory;
  static bool subDir;
  static std::map<std::string, std::string> patternMap;
  static std::vector<std::string> excludeTypes;
  static std::vector<std::string> includeTypes;
  static std::vector<std::string> excludeDir;
  static std::vector<std::string> excludeFiles;
  static std::queue<std::string> includeDir;
  static std::vector<std::map<std::string, std::string>> includeFilesWithPattern;
  static std::string ReplaceWithRegex(std::string input, std::map<std::string, std::string> &mapRef);
  static void callback(const char * directoryPath, _WIN32_FIND_DATAA data);
  static bool CheckAndPush(std::string fileName, std::regex regInstance);

public:
  static ELALogger * logger;
  struct fileNamePatternWithDirStruct
  {
    std::string patternString;
    std::regex patternRegex;
    std::string fileName;
    std::string dirName;
  };
  static std::vector<fileNamePatternWithDirStruct> fileNamePatternDirStructVector;
  static std::vector<std::string> includeFiles;
  static void CreateProcessPattern(std::string patternReference, std::map<std::string, std::string> &mapRef);
  static void ProcessJSON(std::string inputJSON);
};
