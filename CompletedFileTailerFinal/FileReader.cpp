#include <fstream>
#include <string>
#include "ELALogger.h"
#include "DBOperations.h"
std::mutex m;
std::mutex m1;
void ThreadSafeWrite(std::stringstream &threadsafeStringStream)
{
  std::ofstream outFile;
  m.lock();
  outFile.open("D:\\Dir\\TotalOutput.txt",std::ios_base::app);
  outFile << threadsafeStringStream.str();
  outFile.close();
  m.unlock();
}
void ThreadSafeWrite(std::vector<std::string>& vecString)
{
  std::ofstream outFile;
  m.lock();
  outFile.open("D:\\Dir\\TotalOutput.txt", std::ios_base::app);
  for (std::string& s : vecString)
  {
    outFile << s << std::endl;
  }
  outFile.close();
  m.unlock();
}


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
  outFile.open("D:\\Dir\\TotalOutput.txt", std::ios_base::app);
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
  //std::streampos streampos = streamposInt64;
  //std::string fileNameWithoutDir = LoggerUtil::GetFileName(fileName);
  //
  //
  //std::ifstream inputFile;
  //inputFile.open(fileName, std::fstream::in);
  //inputFile.seekg(streampos);
  //std::ofstream outFile;
  //std::string curLine;
  //std::stringstream s;
  //std::vector<std::string> vecString;
  //int stringlength = streampos;
  //if (skipValue != 0)
  //{
  //  std::getline(inputFile, curLine);
  //  stringlength = curLine.length();
  //  streampos += stringlength;
  //}
  //else
  //{
  //  outFile << fileName << std::endl;
  //}
  //std::streampos curPos;
  //int recordCount = 0;
  //
  //while (std::getline(inputFile,curLine))
  //{
  //  recordCount++;
  //  stringlength = curLine.length();
  //  vecString.push_back(std::move(curLine));
  //  if (inputFile.tellg() != -1)
  //  {
  //    streampos = inputFile.tellg();
  //  }
  //  else
  //  {
  //    streampos += stringlength;
  //  }
  //  //s << curLine << std::endl;
  //  if (recordCount == 10000)
  //  {
  //    ThreadSafeWrite(vecString);
  //    vecString.clear();
  //    //ThreadSafeWrite(s);
  //    //s.str(std::string());
  //    curPos = (long long)streampos - stringlength;
  //    DBOperations::UpdateRawQuery("update fileInfo set skip = 1, streampos = "+std::to_string(curPos)+" where fileName = '"+fileName+"'");
  //    recordCount = 0;
  //  }
  //}  
  ////ThreadSafeWrite(s);
  //ThreadSafeWrite(vecString);
  //curPos = (long long)streampos - stringlength;
  //DBOperations::UpdateRawQuery("update fileInfo set skip = 1 , streampos = " + std::to_string(curPos) + " where fileName = '" + fileName+"'");




  //HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  //if(hFile)
  //{
  //  std::string sentence = "";
  //  DWORD bytesWritten;
  //  long long optimalBufferSize = 28200000;
  //  if (WriteFile(hFile,sentence.c_str(),sentence.length(),&bytesWritten,NULL))
  //  {
  //    long long fileSize = LoggerUtil::GetFileDiskSize(fileName);
  //    if (INVALID_SET_FILE_POINTER != SetFilePointer(hFile,streamposInt64,0,FILE_BEGIN))
  //    {
  //      long long readSize;
  //      if(fileSize > optimalBufferSize)
  //      { 
  //        readSize = optimalBufferSize;
  //      }
  //      else
  //      {
  //        readSize = fileSize;
  //      }
  //      char *buf = new char[readSize];
  //      DWORD bytesRead;
  //      HANDLE hFile1 = CreateFile("D:\\Dir\\TotalOutput.txt", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_ALWAYS , FILE_ATTRIBUTE_NORMAL, NULL);
  //     HANDLE hFile1 = CreateFile(("D:\\Dir\\"+fileNameWithoutDir+".txt").c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_ALWAYS , FILE_ATTRIBUTE_NORMAL, NULL);
  //      /*if (WriteFile(hFile1, sentence.c_str(), sentence.length(), &bytesWritten, NULL))
  //      {

  //      }
  //      else
  //      {
  //        std::cout << GetLastError() << " " << std::endl;
  //      }*/
  //      while (true)
  //      {
  //        if (ReadFile(hFile, buf, readSize, &bytesRead, NULL))
  //        {
  //          if (bytesRead != 0)
  //          {
  //            m.lock();
  //            WriteFile(hFile1, sentence.c_str(), sentence.length(), &bytesWritten, NULL);
  //            SetFilePointer(hFile1, 0, 0, FILE_END);
  //            WriteFile(hFile1, buf, bytesRead, &bytesWritten, NULL);
  //            streamposInt64 = streamposInt64 + bytesRead;
  //            DBOperations::UpdateQueury("update fileInfo set skip = ?1 , streampos = ?2 where fileName = ?3", streamposInt64, 1, fileName);
  //            m.unlock();
  //            DBOperations::UpdateRawQuery("update fileInfo set skip = " + std::to_string(skipValue) + " , streampos = " + std::to_string(streamposInt64 + bytesRead) + " where fileName = '" + fileName+"'");
  //            
  //          }
  //          else
  //          {
  //            break;
  //          }
  //        }
  //        else
  //        {
  //          std::cerr << "Failed to ReadFile: " <<
  //            GetLastError() << "\n";
  //        }
  //      }
  //      CloseHandle(hFile1);
  //    }
  //    else
  //    {
  //        std::cerr << "Failed to SetFilePointer: " <<
  //        GetLastError() << "\n";
  //    }

  //  }
  //  else
  //  {
  //    std::cerr << "Failed to WriteFile: " << GetLastError() << "\n";
  //  }

  //  CloseHandle(hFile);
  //}
  //else
  //{
  //  std::cerr << "Failed to open file: " << GetLastError() << "\n";
  //}


  FILE * inputfptr;
  FILE * outputftr;
  inputfptr = fopen(fileName.c_str(),"r+" );
  //outputftr = fopen("D:\\Dir\\TotalOutput.txt", "a+");
  fseek(inputfptr, streamposInt64, FILE_BEGIN);
  std::stringstream s;
  std::vector<std::string> vecString;
  char curLine[200];
  int stringlength = 0;
  if (skipValue != 0)
  {
    fscanf(inputfptr,"%[^\n]\n" ,curLine);
    stringlength = strlen(curLine);
  }
  else
  {
    std::cout << fileName << std::endl;
    //fprintf(outputftr, "%s", fileName);
  }
  std::streampos curPos;
  int recordCount = 0;
  while (fscanf(inputfptr,"%[^\n]\n" ,curLine)!=EOF)
  {
    recordCount++;
    vecString.push_back(std::move(curLine));
    //s << curLine << std::endl;
    stringlength = strlen(curLine);
    if (recordCount == 10000)
    {
      ThreadSafeWrite(vecString);
      vecString.clear();
      //ThreadSafeWrite(s);
      //s.str(std::string());
      curPos = ftell(inputfptr) - stringlength;
      DBOperations::UpdateRawQuery("update fileInfo set skip = 1 , streampos = " + std::to_string(curPos) + " where fileName = '" + fileName+"'");
      recordCount = 0;
    }
  } 
  ThreadSafeWrite(vecString);
  //ThreadSafeWrite(s);
  fseek(inputfptr, -stringlength, FILE_END);
  curPos = ftell(inputfptr);
  DBOperations::UpdateRawQuery("update fileInfo set skip = 1 , streampos = " + std::to_string(curPos) + " where fileName = '" + fileName+"'");

}