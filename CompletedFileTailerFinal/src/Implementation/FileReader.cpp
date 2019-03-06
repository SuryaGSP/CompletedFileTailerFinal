#include <fstream>
#include <string>
#include "ELALogger.h"
#include "DBOperations.h"

void readRenamedFileData(std::string fileName, std::string renamedFileName)
{
  long long streamposInt64 = 0;
  int skipValue = 0;
  DBOperations::RetrieveSPosAndSkip("select streampos,skip from fileInfo where fileName = ?1", fileName, streamposInt64, skipValue);
  std::streampos streampos = streamposInt64;
  std::fstream inputFile;
  inputFile.open(renamedFileName, std::fstream::in);
  inputFile.seekg(streampos);
  std::ofstream outFile;
  std::string curLine;
  std::string fileNameWithoutDir = LoggerUtil::GetFileName(fileName);
  outFile.open("D:\\Dir\\" + fileNameWithoutDir + ".txt", std::ios_base::app);
  char *fileNamechar = new char[fileName.length() + 1];
  std::strcpy(fileNamechar, fileName.c_str());
  outFile << fileName << std::endl;
  std::stringstream s;
  while (std::getline(inputFile, curLine))
  {
    s << curLine << std::endl;
  }
  outFile << s.str();
}

void ReadFile(std::string fileName)
{
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
  std::stringstream s;
  outFile.open("D:\\Dir\\" + fileNameWithoutDir + ".txt", std::ios_base::app);
  if (skipValue != 0)
  {
    std::getline(inputFile, curLine);
  }
  else
  {
    outFile << fileName << std::endl;
  }
  std::streampos curPos;
  int recordCount = 0;
  while (inputFile.peek() != -1)
  {
    curPos = inputFile.tellg();
    std::getline(inputFile, curLine);
    s << curLine << std::endl;
    recordCount++;
    if (inputFile.peek() == -1 || recordCount >= 1000)
    {
      outFile << s.str();
      s.str(std::string());
      DBOperations::UpdateQueury("update fileInfo set skip = ?1 , streampos = ?2 where fileName = ?3", curPos, 1, fileName);
      recordCount = 0;
    }
  }
}