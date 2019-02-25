#include "JSONProcessor.h"
#include "FileInfo.h"
#include "HelperFunctions.h"

bool JSONProcessor::subDir;
struct JSONProcessor::fileNamePatternWithDirStruct;
std::string JSONProcessor::patternForDirectory;
std::map<std::string, std::string> JSONProcessor::patternMap;
std::vector<std::string> JSONProcessor::excludeTypes;
std::vector<std::string> JSONProcessor::includeTypes;
std::vector<std::string> JSONProcessor::excludeDir;
std::vector<std::string> JSONProcessor::excludeFiles;
std::queue<std::string> JSONProcessor::includeDir;
std::vector<std::string> JSONProcessor::includeFiles;
std::vector<std::map<std::string, std::string>> JSONProcessor::includeFilesWithPattern;
std::vector<JSONProcessor::fileNamePatternWithDirStruct> JSONProcessor::fileNamePatternDirStructVector;
void processFiles(std::vector<JSONProcessor::fileNamePatternWithDirStruct> &fileNameStructvec);
bool JSONProcessor::CheckAndPush(std::string fileName, std::regex regInstance)
{
  std::string fileNameWithoutExt = LoggerUtil::GetFileNameWithOutExtension(fileName);
  std::smatch m;
  bool value = std::regex_match(fileNameWithoutExt, m, regInstance);
  return value;
}

std::string JSONProcessor::ReplaceWithRegex(std::string input, std::map<std::string, std::string> &mapRef)
{
  std::string st = "(\\$\\{[^\\}]+\\})";
  std::regex regInstance(st);
  std::smatch match;
  int i = 0;
  std::string replace = "";
  while (std::regex_search(input, match, regInstance))
  {
    input.replace(input.find(match.str(i), 0), match.str(i).length(), mapRef[match.str(i)]);
  }
  return ("(" + input + ")");
}

void  JSONProcessor::callback(const char * direcPath, _WIN32_FIND_DATAA data)
{
  std::string directoryPath = direcPath;
  std::string name = data.cFileName;
  if (data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
  {
    if (find(excludeFiles.begin(), excludeFiles.end(), directoryPath + "*") == excludeFiles.end())
    {
      if (find(excludeDir.begin(), excludeDir.end(), directoryPath + name + "\\") == excludeDir.end())
      {
        includeDir.push(directoryPath + name + "\\");
      }
    }
  }
  else
  {
    if (find(excludeFiles.begin(), excludeFiles.end(), data.cFileName) == excludeFiles.end())
    {
      if (includeTypes.size() == 0 && excludeTypes.size() == 0)
      {
        if (patternForDirectory == "")
        {
          if (FileInfo::FindDirInMonitorDir(directoryPath))
          {
            FileInfo::PushMonitorDirectory(directoryPath);
            FileInfo::LoadDirectoryMap(directoryPath, includeTypes, excludeTypes, ReplaceWithRegex(patternForDirectory, patternMap));
          }
          includeFiles.push_back(directoryPath + data.cFileName);
        }
        else
        {
          std::regex regInstance(ReplaceWithRegex(patternForDirectory, patternMap));
          if (CheckAndPush(data.cFileName, regInstance))
          {
            if (FileInfo::FindDirInMonitorDir(directoryPath))
            {
              FileInfo::LoadDirectoryMap(directoryPath, includeTypes, excludeTypes, ReplaceWithRegex(patternForDirectory, patternMap));
              FileInfo::PushMonitorDirectory(directoryPath);
            }
            fileNamePatternWithDirStruct fileStructInstance;
            fileStructInstance.dirName = directoryPath;
            fileStructInstance.fileName = data.cFileName;
            fileStructInstance.patternRegex = regInstance;
            fileStructInstance.patternString = patternForDirectory;
            fileNamePatternDirStructVector.push_back(fileStructInstance);

          }
        }
      }
      else
      {
        std::string fileType = "." + LoggerUtil::GetExtension(name);
        if ((includeTypes.size() > 0) && (find(includeTypes.begin(), includeTypes.end(), fileType) != includeTypes.end()))
        {
          if (patternForDirectory == "")
          {
            if (FileInfo::FindDirInMonitorDir(directoryPath))
            {
              FileInfo::LoadDirectoryMap(directoryPath, includeTypes, excludeTypes, ReplaceWithRegex(patternForDirectory, patternMap));
              FileInfo::PushMonitorDirectory(directoryPath);
            }
            includeFiles.push_back(directoryPath + data.cFileName);
          }
          else
          {
            std::regex regInstance(ReplaceWithRegex(patternForDirectory, patternMap));

            if (CheckAndPush(data.cFileName, regInstance))
            {
              if (FileInfo::FindDirInMonitorDir(directoryPath))
              {
                FileInfo::LoadDirectoryMap(directoryPath, includeTypes, excludeTypes, ReplaceWithRegex(patternForDirectory, patternMap));
                FileInfo::PushMonitorDirectory(directoryPath);
              }
              // Here
              fileNamePatternWithDirStruct fileStructInstance;
              fileStructInstance.dirName = directoryPath;
              fileStructInstance.fileName = data.cFileName;
              fileStructInstance.patternRegex = regInstance;
              fileStructInstance.patternString = patternForDirectory;
              fileNamePatternDirStructVector.push_back(fileStructInstance);
            }
          }
        }
        else if ((excludeTypes.size() > 0) && (find(excludeTypes.begin(), excludeTypes.end(), fileType) == excludeTypes.end()))
        {
          if (patternForDirectory == "")
          {
            if (FileInfo::FindDirInMonitorDir(directoryPath))
            {
              FileInfo::LoadDirectoryMap(directoryPath, includeTypes, excludeTypes, ReplaceWithRegex(patternForDirectory, patternMap));
              FileInfo::PushMonitorDirectory(directoryPath);
            }
            includeFiles.push_back(directoryPath + data.cFileName);
          }
          else
          {
            std::regex regInstance(ReplaceWithRegex(patternForDirectory, patternMap));
            if (CheckAndPush(data.cFileName, regInstance))
            {
              if (FileInfo::FindDirInMonitorDir(directoryPath))
              {
                FileInfo::LoadDirectoryMap(directoryPath, includeTypes, excludeTypes, ReplaceWithRegex(patternForDirectory, patternMap));
                FileInfo::PushMonitorDirectory(directoryPath);
              }
              // Here
              fileNamePatternWithDirStruct fileStructInstance;
              fileStructInstance.dirName = directoryPath;
              fileStructInstance.fileName = data.cFileName;
              fileStructInstance.patternRegex = regInstance;
              fileStructInstance.patternString = patternForDirectory;
              fileNamePatternDirStructVector.push_back(fileStructInstance);
            }
          }
        }
      }
    }
  }
}


void JSONProcessor::ProcessJSON(std::string inputJSON)
{
  std::string patternReference = "[{ \"NAME\": \"Day Pattern\", \"HASCHILD\": true, \"LIST\": [{ \"NAME\": \"1,2,3,4...\", \"DATA\": \"${d}\", \"regex\": \"([1-9]|[12]\\\\d|3[01])\" }, { \"NAME\": \"01,02,03...\", \"DATA\": \"${dd}\", \"regex\": \"(0[1-9]|[12]\\\\d|3[01])\" }] }, { \"NAME\": \"Month Pattern\", \"HASCHILD\": true, \"LIST\": [{ \"NAME\": \"1,2,3,4...\", \"DATA\": \"${M}\", \"regex\": \"([1-9]|1[12])\" }, { \"NAME\": \"01,02,03...\", \"DATA\": \"${MM}\", \"regex\": \"(0[1-9]|1[12])\" }, { \"NAME\": \"JAN,FEB,MAR...\", \"DATA\": \"${MMM}\", \"regex\": \"jan|feb|mar|apr|may|jun|jul|aug|sep|oct|nov|dec\" }, { \"NAME\": \"JANUARY,FEBRUARY...\", \"DATA\": \"${MMMM}\", \"regex\": \"january|february|march|april|may|june|july|august|september|october|november|december\" }] }, { \"NAME\": \"Year Pattern\", \"HASCHILD\": true, \"LIST\": [{ \"NAME\": \"01,02,03...\", \"DATA\": \"${yy}\", \"regex\": \"(\\\\d{2})\" }, { \"NAME\": \"2001,2002...\", \"DATA\": \"${yyyy}\", \"regex\": \"(\\\\d{4})\" }] }, { \"NAME\": \"Weekday Pattern\", \"HASCHILD\": true, \"LIST\": [{ \"NAME\": \"Sun,Mon,Tue...\", \"DATA\": \"${EEE}\", \"regex\": \"sun|mon|tue|wed|thu|fri|sat\" }, { \"NAME\": \"Sunday,Mondays...\", \"DATA\": \"${EEEE}\", \"regex\": \"sunday|monday|tuesday|wednesday|thursday|friday\" }] }, { \"NAME\": \"Week in Year Pattern\", \"HASCHILD\": true, \"LIST\": [{ \"NAME\": \"1,2,3...\", \"DATA\": \"${w}\", \"regex\": \"([1-9]|[1234]\\\\d|5[012])\" }, { \"NAME\": \"01,02...\", \"DATA\": \"${ww}\", \"regex\": \"(0[1-9]|[1234]\\\\d|5[012])\" }] }, { \"NAME\": \"Week In Month Pattern\", \"HASCHILD\": true, \"LIST\": [{ \"NAME\": \"1,2,3...\", \"DATA\": \"${W}\", \"regex\": \"([1234])\" }, { \"NAME\": \"01,02...\", \"DATA\": \"${WW}\", \"regex\": \"(0[1234])\" }] }, { \"NAME\": \"Hour Pattern\", \"HASCHILD\": true, \"LIST\": [{ \"NAME\": \"1,2...24\", \"DATA\": \"${H}\", \"regex\": \"([1-9]|[1]\\\\d|2[01234])\" }, { \"NAME\": \"01,02...24\", \"DATA\": \"${HH}\", \"regex\": \"(0[1-9]|[1]\\\\d|2[01234])\" }] }, { \"NAME\": \"Minute Pattern\", \"HASCHILD\": true, \"LIST\": [{ \"NAME\": \"1,2,3,4...\", \"DATA\": \"${m}\", \"regex\": \"([1-9]|[12345]\\\\d)\" }, { \"NAME\": \"01,02...\", \"DATA\": \"${mm}\", \"regex\": \"(0[1-9]|[12345]\\\\d)\" }] }, { \"NAME\": \"Second Pattern\", \"HASCHILD\": true, \"LIST\": [{ \"NAME\": \"1,2,3,4...\", \"DATA\": \"${s}\", \"regex\": \"([1-9]|[12345]\\\\d)\" }, { \"NAME\": \"01,02...\", \"DATA\": \"${ss}\", \"regex\": \"(0[1-9]|[12345]\\\\d)\" }] }, { \"NAME\": \"Number of Increments Pattern\", \"HASCHILD\": true, \"LIST\": [{ \"NAME\": \"1,2,3,4...\", \"DATA\": \"${{NUMBER_1}}\", \"regex\": \"(\\\\d+)\" }, { \"NAME\": \"01,02,03...\", \"DATA\": \"${{NUMBER_2}}\", \"regex\": \"(0[0-9]|[1-9]\\\\d+)\" }, { \"NAME\": \"001,002,003...\", \"DATA\": \"${{NUMBER_3}}\", \"regex\": \"(00[0-9]|0[1-9][0-9]|[1-9][0-9]\\\\d+)\" }] }]";
  CreateProcessPattern(patternReference, patternMap);
  rapidjson::Document d;
  d.Parse(inputJSON.c_str());
  if (d.IsArray())
  {
    for (const rapidjson::Value& outerArray : d.GetArray())
    {
      if (outerArray.HasMember("locations"))
      {
        for (const rapidjson::Value& innerLocationsArray : outerArray["locations"].GetArray())
        {
          subDir = true;
          excludeTypes.clear();
          includeTypes.clear();
          fileNamePatternDirStructVector.clear();
          if (innerLocationsArray.HasMember("excludetypes"))
          {
            for (const rapidjson::Value & innerExcludeTypesArray : innerLocationsArray["excludetypes"].GetArray())
            {
              excludeTypes.push_back(innerExcludeTypesArray.GetString());
            }
          }
          if (innerLocationsArray.HasMember("includetypes"))
          {
            for (const rapidjson::Value& innerIncludeTyepesArray : innerLocationsArray["includetypes"].GetArray())
            {
              includeTypes.push_back(innerIncludeTyepesArray.GetString());
            }
          }
          if (innerLocationsArray.HasMember("exclude"))
          {
            for (const rapidjson::Value & innerExcludeArray : innerLocationsArray["exclude"].GetArray())
            {
              std::string curName = innerExcludeArray.GetString();
              if (GetFileAttributes(curName.c_str()) == FILE_ATTRIBUTE_DIRECTORY)
              {
                excludeDir.push_back(curName);
              }
              else
              {
                if (curName == "*")
                {
                  subDir = false;
                }
                else
                {
                  excludeFiles.push_back(curName);
                }
              }
            }
          }
          if (innerLocationsArray.HasMember("include"))
          {
            std::string dirName = innerLocationsArray["include"].GetString();
            if (GetFileAttributes(dirName.c_str()) == FILE_ATTRIBUTE_DIRECTORY)
            {
              includeDir.push(dirName);
              patternForDirectory = "";
              if (innerLocationsArray.HasMember("pattern"))
              {
                patternForDirectory = innerLocationsArray["pattern"].GetString();
              }
              while (!includeDir.empty())
              {
                std::string curDir = includeDir.front();
                SystemUtil::ForEachMatchingFile(callback, curDir.c_str(), "");
                includeDir.pop();
                if (subDir == false)
                {
                  break;
                }
              }
              if (fileNamePatternDirStructVector.size() > 0)
              {
                processFiles(fileNamePatternDirStructVector);
              }
            }
            else
            {
              includeFiles.push_back(dirName);
              dirName = LoggerUtil::GetParent(dirName);
              FileInfo::PushMonitorDirectory(dirName);
            }
          }
        }
      }
      else
      {
        std::cout << "Has no member Locations";
      }
    }
    for (std::string &s : includeFiles)
    {
      FileInfo::AddFileNamesMonitoredFirst(s);
    }
  }
  else
  {
    std::cout << inputJSON;
  }
}

void JSONProcessor::CreateProcessPattern(std::string patternReference, std::map<std::string, std::string>& mapRef)
{
  rapidjson::Document d1;
  d1.Parse(patternReference.c_str());
  if (d1.IsArray())
  {
    for (const rapidjson::Value& outerArray : d1.GetArray())
    {
      if (outerArray.HasMember("LIST"))
      {
        for (const rapidjson::Value& innerListArray : outerArray["LIST"].GetArray())
        {
          mapRef[innerListArray["DATA"].GetString()] = innerListArray["regex"].GetString();
        }
      }
      else
      {
        std::cout << "Invalid Input for Pattern Reference...." << std::endl;
      }
    }
  }
  else
  {
    std::cout << "Invalid Input for Pattern Reference...." << std::endl;
  }
}