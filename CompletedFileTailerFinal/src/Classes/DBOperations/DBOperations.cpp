#include "DBOperations.h"


sqlite3 *DBOperations::DBObject = NULL;
void DBOperations::OpenConnection(std::string dbNameWithoutDotDB)
{
  if (sqlite3_open((dbNameWithoutDotDB + ".db").c_str(), &DBObject) == SQLITE_OK)
  {
    std::cout << "connection established";
  }
  else
  {
    std::cout << "Database Connection failed";
  }
}

void DBOperations::CloseConnection()
{
  if (sqlite3_close(DBObject) == SQLITE_OK)
  {
    std::cout << "Connection closed";
  }
  else
  {
    std::cout << "Connection termination failed";
  }
}

void DBOperations::createTableIfNotExists(std::string query)
{
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(DBObject, query.c_str(), -1, &stmt, NULL);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

void DBOperations::UpdateQueury(std::string sql, long long int streampos, int SkipValue, std::string fileName)
{
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(DBObject, sql.c_str(), -1, &stmt, NULL);
  sqlite3_bind_int64(stmt, 2, streampos);
  sqlite3_bind_int(stmt, 1, SkipValue);
  sqlite3_bind_text(stmt, 3, fileName.c_str(), -1, SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

void DBOperations::UpdateQuery(std::string sql, std::string fileName, std::string dirName)
{
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(DBObject, sql.c_str(), -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, fileName.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, dirName.c_str(), -1, SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}


void DBOperations::RetrieveSPosAndSkip(std::string sql, std::string fileName, long long & streampos, int & skipValue)
{
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(DBObject, sql.c_str(), -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, fileName.c_str(), -1, SQLITE_STATIC);
  if (sqlite3_step(stmt) != SQLITE_DONE)
  {
    streampos = sqlite3_column_int64(stmt, 0);
    skipValue = sqlite3_column_int(stmt, 1);

  }
  sqlite3_finalize(stmt);
  return;
}
bool DBOperations::checkRowCount(std::string sql, std::string dirName)
{
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(DBObject, sql.c_str(), -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 2, dirName.c_str(), -1, SQLITE_STATIC);
  bool success = true;
  sqlite3_step(stmt);
  int count = sqlite3_column_int(stmt, 0);
  if (count == 0)
    success = false;
  sqlite3_finalize(stmt);
  return success;
}
void DBOperations::RetrieveLastFileName(std::string sql, std::string dirName, std::string &lastFileName)
{
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(DBObject, sql.c_str(), -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 2, dirName.c_str(), -1, SQLITE_STATIC);
  if (sqlite3_step(stmt) != SQLITE_DONE)
  {
    lastFileName = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
  }
  sqlite3_finalize(stmt);
  return;
}
void DBOperations::InsertQuery(std::string sql, std::string fileName, int skipValue, long long streampos)
{
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(DBObject, sql.c_str(), -1, &stmt, NULL);
  sqlite3_bind_int64(stmt, 2, streampos);
  sqlite3_bind_int(stmt, 3, skipValue);
  sqlite3_bind_text(stmt, 1, fileName.c_str(), -1, SQLITE_STATIC);
  if (sqlite3_step(stmt) == SQLITE_DONE)
  {
  }
  else
  {
    std::cout << "Insert Failed" << std::endl;
  }
  sqlite3_finalize(stmt);
}

void DBOperations::InsertQueryMaintainState(std::string sql, std::string dirName, std::string fileName1)
{
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(DBObject, sql.c_str(), -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 2, dirName.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, fileName1.c_str(), -1, SQLITE_STATIC);
  if (sqlite3_step(stmt) == SQLITE_DONE)
  {
  }
  else
  {
    std::cout << "Insert Failed" << std::endl;
  }
  sqlite3_finalize(stmt);
}



