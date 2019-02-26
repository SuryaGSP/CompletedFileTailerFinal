#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include "JSONProcessor.h"
#include "DBOperations.h"
#include "ThreadSafeQueue.h"
#include "ELALogger.h"
#include "CalenderHelper.h"
void ReadFileFromQueue(std::string fileName)
{
  std::cout << "Readed " << fileName << std::endl;
  // std::this_thread::sleep_for(std::chrono::milliseconds(500));
  long long streamposInt64;
  int skipValue;
  DBOperations::RetrieveSPosAndSkip("select streampos,skip from fileInfo where fileName = ?1", fileName, streamposInt64, skipValue);
  std::streampos streampos = streamposInt64;
  char *zErrMsg = 0;
  std::ifstream inputFile;
  inputFile.open(fileName, std::fstream::in);
  inputFile.seekg(streampos);
  std::ofstream outFile;
  std::string curLine;
  std::string fileNameWithoutDir = LoggerUtil::GetFileName(fileName);
  ELALogger *logger = new ELALogger();
  logger->SetExtension("txt");
  logger->SetLogFileName(fileNameWithoutDir);
  logger->SetLogDir("D:\\Dir");
  logger->Start();
  if (skipValue != 0)
  {
    std::getline(inputFile, curLine);
  }
  else
  {
    logger->info(&fileName[0u]);
  }
  std::streampos curPos;
  int recordCount = 0;
  while (inputFile.peek() != -1)
  {
    curPos = inputFile.tellg();
    std::getline(inputFile, curLine);
    logger->info(&curLine[0u]);
    recordCount++;
    if (inputFile.peek() == -1 || recordCount >= 1000)
    {
      DBOperations::UpdateQueury("update fileInfo set skip = ?1 , streampos = ?2 where fileName = ?3", curPos, 1, fileName);
      //std::cout << "updated " << LoggerUtil::GetParent(fileName) << LoggerUtil::GetFileName(fileName) << std::endl;
      DBOperations::UpdateQuery("update dirFilePatternInfo set fileName = ?1 where dirName = ?2", LoggerUtil::GetFileName(fileName), LoggerUtil::GetParent(fileName) + "\\");
      recordCount = 0;
    }
  }
}

ThreadSafeQueue<std::string>* getPatternQueueObject()
{
  static ThreadSafeQueue<std::string> * patternInstance;
  if (!patternInstance)
    patternInstance = new ThreadSafeQueue<std::string>();
  return patternInstance;
}

void WorkerThread()
{
  ThreadSafeQueue<std::string> * modifiedInstance = getPatternQueueObject();
  std::string fileName;
  while (modifiedInstance->TryPop(fileName))
  {
    ReadFileFromQueue(fileName);
  }
}


bool compareDates(JSONProcessor::fileNamePatternWithDirStruct first, JSONProcessor::fileNamePatternWithDirStruct second)
{
  std::string firstFileNameWithoutExt = LoggerUtil::GetFileNameWithOutExtension(first.fileName);
  std::string secondFileNameWithoutExt = LoggerUtil::GetFileNameWithOutExtension(second.fileName);
  std::smatch firstStringMatch, secondStringMatch, firstTypeMatch, secondTypeMatch;
  std::regex_match(firstFileNameWithoutExt, firstStringMatch, first.patternRegex);
  std::regex_match(secondFileNameWithoutExt, secondStringMatch, second.patternRegex);
  std::map<std::string, std::string > firstMap, secondMap;
  int i = 0;
  std::string st1 = "(\\$\\{[^\\}]+\\})";
  std::regex st(st1);
  while (std::regex_search(first.patternString, firstTypeMatch, st))
  {
    firstMap[firstTypeMatch.str(0)] = firstStringMatch.str(i + 2);
    first.patternString.replace(first.patternString.find(firstTypeMatch.str(0), 0), firstTypeMatch.str(0).length(), "");
    i++;
  }
  i = 0;
  while (std::regex_search(second.patternString, secondTypeMatch, st))
  {
    secondMap[secondTypeMatch.str(0)] = secondStringMatch.str(i + 2);
    second.patternString.replace(second.patternString.find(secondTypeMatch.str(0), 0), secondTypeMatch.str(0).length(), "");
    i++;
  }
  if (firstMap.find("${yyyy}") != firstMap.end() || firstMap.find("${yy}") != firstMap.end())
  {
    if(firstMap["${yyyy}"] != secondMap["${yyyy}"])
      return (stoi(firstMap["${yyyy}"]) < stoi(secondMap["${yyyy}"]));
    else if (firstMap["${yy}"] != secondMap["${yy}"])
      return (stoi(firstMap["${yy}"]) < stoi(secondMap["${yy}"]));
    else if (firstMap["${M}"] != secondMap["${M}"])
      return (stoi(firstMap["${M}"]) < stoi(secondMap["${M}"]));
    else if (firstMap["${MM}"] != secondMap["${MM}"])
      return (stoi(firstMap["${MM}"]) < stoi(secondMap["${MM}"]));
    else if (firstMap["${MMM}"] != secondMap["${MMM}"])
    {
      if (CalenderHelper::monthPatternShort.size() == 0)
        CalenderHelper::setmonthPatternShort();
      return CalenderHelper::monthPatternShort[firstMap["${MMM}"]]< CalenderHelper::monthPatternShort[secondMap["${MMM}"]];
    }
    else if (firstMap["${MMMM}"] != secondMap["${MMMM}"])
    {
      if (CalenderHelper::monthPatternLong.size() == 0)
        CalenderHelper::setMOnthPatternLong();
      return CalenderHelper::monthPatternLong[firstMap["${MMM}"]] < CalenderHelper::monthPatternLong[secondMap["${MMM}"]];
    }
    else if (firstMap["${dd}"] != secondMap["${dd}"])
      return (stoi(firstMap["${dd}"]) < stoi(secondMap["${dd}"]));
    else if (firstMap["${d}"] != secondMap["${d}"])
      return (stoi(firstMap["${d}"]) < stoi(secondMap["${d}"]));
  }
  if (firstMap.find("${H}") != firstMap.end() || firstMap.find("${HH}") != firstMap.end())
  {
    if (firstMap["${H}"] != secondMap["${H}"])
      return (stoi(firstMap["${H}"]) < stoi(secondMap["${H}"]));
    else if (firstMap["${HH}"] != secondMap["${HH}"])
      return (stoi(firstMap["${HH}"]) < stoi(secondMap["${HH}"]));
    else if (firstMap["${m}"] != secondMap["${m}"])
      return (stoi(firstMap["${m}"]) < stoi(secondMap["${m}"]));
    else if (firstMap["${mm}"] != secondMap["${mm}"])
      return (stoi(firstMap["${mm}"]) < stoi(secondMap["${mm}"]));
    else if (firstMap["${s}"] != secondMap["${s}"])
      return (stoi(firstMap["${s}"]) < stoi(secondMap["${s}"]));
    else if (firstMap["${ss}"] != secondMap["${ss}"])
      return (stoi(firstMap["${ss}"]) < stoi(secondMap["${ss}"]));
  }
  else if (firstMap.find("${NUMBER_1}") != firstMap.end() || firstMap.find("${NUMBER_2}") != firstMap.end() || firstMap.find("${NUMBER_3}") != firstMap.end())
  {
    if (firstMap["${NUMBER_1}"] != secondMap["${NUMBER_1}"])
      return (stoi(firstMap["${NUMBER_1}"]) < stoi(secondMap["${NUMBER_1}"])); 
    if (firstMap["${NUMBER_2}"] != secondMap["${NUMBER_2}"])
      return (stoi(firstMap["${NUMBER_2}"]) < stoi(secondMap["${NUMBER_2}"]));   
    if (firstMap["${NUMBER_3}"] != secondMap["${NUMBER_3}"])
      return (stoi(firstMap["${NUMBER_3}"]) < stoi(secondMap["${NUMBER_3}"]));
  }
  else if (firstMap.find("${w}") != firstMap.end() || firstMap.find("${ww}") != firstMap.end() || firstMap.find("${W}") != firstMap.end() || firstMap.find("${WW}") != firstMap.end())
  {
    if (firstMap["${w}"] != secondMap["${w}"])
      return (stoi(firstMap["${w}"]) < stoi(secondMap["${w}"]));
    if (firstMap["${ww}"] != secondMap["${ww}"])
      return (stoi(firstMap["${ww}"]) < stoi(secondMap["${ww}"]));  
    if (firstMap["${W}"] != secondMap["${W}"])
      return (stoi(firstMap["${W}"]) < stoi(secondMap["${W}"]));
    if (firstMap["${WW}"] != secondMap["${WW}"])
      return (stoi(firstMap["${WW}"]) < stoi(secondMap["${WW}"]));
  }
  else if(firstMap.find("${EEE}") != firstMap.end()|| firstMap.find("${EEEE}") != firstMap.end())
  {
    if (firstMap["${EEE}"] != secondMap["${EEE}"])
    {
      if (CalenderHelper::weekDayPatternShort.size() == 0)
        CalenderHelper::setWeekDayPatternShort();
      return CalenderHelper::weekDayPatternShort[firstMap["${EEE}"]] < CalenderHelper::weekDayPatternShort[secondMap["${EEE}"]];
    }
    else 
    {
      if (CalenderHelper::weekDayPatternLong.size() == 0)
        CalenderHelper::setWeekDayPatternLong();
      return CalenderHelper::weekDayPatternLong[firstMap["${EEE}"]] < CalenderHelper::weekDayPatternLong[secondMap["${EEE}"]];
    }
  }
  return false;
}


void processFiles(std::vector<JSONProcessor::fileNamePatternWithDirStruct> &fileNameStructvec)
{
  std::sort(fileNameStructvec.begin(), fileNameStructvec.end(), compareDates);
  JSONProcessor::fileNamePatternWithDirStruct vecfirst = *fileNameStructvec.begin();
  std::cout << vecfirst.dirName << std::endl;
  if (DBOperations::checkRowCount("select count(*) from dirFilePatternInfo where dirName = ?2", vecfirst.dirName))
  {
    std::cout << vecfirst.dirName << "rows found check and push" << std::endl;
    std::string lastFileName;
    DBOperations::RetrieveLastFileName("select fileName from dirFilePatternInfo where dirName = ?2", vecfirst.dirName, lastFileName);
    bool notSkip = false;
    for (JSONProcessor::fileNamePatternWithDirStruct ce : fileNameStructvec)
    {
      if (ce.fileName == lastFileName)
      {
        notSkip = true;
        std::cout << "match found :: " << ce.fileName << std::endl;
        DBOperations::InsertQuery("insert or ignore into fileInfo values(?1,?2,?3)", ce.dirName + ce.fileName, 0, 0);
        getPatternQueueObject()->Push(ce.dirName + ce.fileName);
      }
      else if (notSkip == true)
      {
        std::cout << " found file " << ce.fileName << std::endl;
        DBOperations::InsertQuery("insert or ignore into fileInfo values(?1,?2,?3)", ce.dirName + ce.fileName, 0, 0);
        getPatternQueueObject()->Push(ce.dirName + ce.fileName);
      }
      else
      {
        std::cout << "skipped " << ce.fileName << std::endl;
      }
    }
  }
  else
  {
    std::cout << vecfirst.dirName << " no rows found push sorted fileNames" << vecfirst.fileName << std::endl;
    DBOperations::InsertQueryMaintainState("insert into dirFilePatternInfo values(?2,?3)", vecfirst.dirName, vecfirst.fileName);
    for (JSONProcessor::fileNamePatternWithDirStruct ce : fileNameStructvec)
    {
      DBOperations::InsertQuery("insert or ignore into fileInfo values(?1,?2,?3)", ce.dirName + ce.fileName, 0, 0);
      getPatternQueueObject()->Push(ce.dirName + ce.fileName);
    }
  }
  std::thread t1(WorkerThread);
  t1.join();
}