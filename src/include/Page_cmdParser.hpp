#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <string>
#include "Database.hpp"

class PageParser
{
  uint32_t page_no;
  u_int64_t page_offset; // Initialised using (page_no-1)*db.page_size
  uint16_t cellCount;
  std::vector<uint16_t> cellPointers;
  Database &db;
  ByteReader Br;
  std::vector<bool> page_type; // (true,true) for Interior Table, (false,false) for Leaf Index
  std::vector<std::string> columns;

public:
  uint16_t getCellCount();
  uint64_t sizeBySerialType(uint64_t stc);
  PageParser(Database &db, uint32_t page_no, std::vector<std::string> columns = {});

  // schema functions
  void printUserTableNames();
  uint32_t pageNoByTblName(std::string &tblName);
  uint32_t columnNames_pageNo(std::string &tblName, std::vector<std::string> &columns);

  // generic functions
  void getColumn(std::string &column);
};

class CommandParser
{
public:
  Database &db;
  std::string command;
  std::vector<std::string> splitted;
  std::string query;
  std::vector<
      std::string>
  split(std::string &s, char delim);
  CommandParser(Database &db, std::string cmd);

  void taskByQuery();
  void getColumnsFromCreateTable(std::vector<std::string> &columns);
};
