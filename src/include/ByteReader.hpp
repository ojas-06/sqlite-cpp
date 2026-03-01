#pragma once

#include <cstring>
#include <iostream>
#include <fstream>
#include <utility>

class Database;

class ByteReader
{
public:
  void read_at(std::ifstream &database_file,
               char *buffer,
               unsigned int size,
               unsigned int offset,
               unsigned int pre_off = 0);
  uint16_t byteToU16(char *buffer);

  std::pair<uint64_t, int> read_varint(Database &db, uint64_t offset);
};
