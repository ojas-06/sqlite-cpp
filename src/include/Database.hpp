#pragma once
#include "ByteReader.hpp"

class Database
{
public:
  std::ifstream  database_file;
  uint16_t       page_size;
  uint16_t       tables;
  ByteReader     Br;

  explicit       Database(const std::string &db_filepath);
  void           setDbInfo();
  void           getDbInfo();
  std::ifstream& getDBFile();
};