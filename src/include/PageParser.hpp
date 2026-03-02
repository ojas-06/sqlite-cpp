#pragma once
#include "./Database.hpp"
#include "./ByteReader.hpp"
#include <vector>
#include <tuple>

class PageParser
{
  uint32_t               page_no;
  u_int64_t              page_offset; // Initialised using (page_no-1)*db.page_size
  uint16_t               cellCount;
  std::vector<uint16_t>  cellPointers;
  Database&              db;
  ByteReader             Br;
  std::vector<bool>      page_type; // (true,true) for Interior Table, (false,false) for Leaf Index
public:
  PageParser(Database& db, unsigned int page_no);
  void printUserTableNames();
};