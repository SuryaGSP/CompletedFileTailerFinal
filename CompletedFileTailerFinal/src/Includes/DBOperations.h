#pragma once
#include <iostream>
#include "sqlite3.h"
#include "ELALogger.h"
class DBOperations
{
  static sqlite3 *DBObject;
  static ELALogger * logObject;
public:
  static void OpenConnection(std::string dbNameWithoutDotDB);
  static void CloseConnection();
  static void createTableIfNotExists(std::string query);
  static void UpdateQueury(std::string sql, long long int streampos, int SkipValue, std::string fileName);
  static void UpdateQuery(std::string sql, std::string fileName, std::string dirName);
  static void UpdateRawQuery(std::string sql);
  static void InsertQuery(std::string sql, std::string fileName, int skipValue, long long streampos);
  static void InsertQueryMaintainState(std::string sql, std::string dirName, std::string fileName1);
  static void RetrieveSPosAndSkip(std::string sql, std::string fileName, long long &streampos, int &skipValue);
  static void RetrieveLastFileName(std::string sql, std::string dirName, std::string &lastFileName);
  static bool checkRowCount(std::string sql, std::string dirName);
};
