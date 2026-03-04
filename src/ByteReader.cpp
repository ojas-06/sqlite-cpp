#include "./include/ByteReader.hpp"
#include "./include/Database.hpp"

void ByteReader::read_at(std::ifstream &database_file, char *buffer, unsigned int size, unsigned int offset, unsigned int pre_off)
{
  database_file.seekg(offset + pre_off);
  database_file.read(buffer, size);
}

uint16_t ByteReader::byteToU16(char *buffer)
{
  return (static_cast<unsigned char>(buffer[1]) | (static_cast<unsigned char>(buffer[0]) << 8));
}
uint64_t ByteReader::byteToU64(char *buffer, int size) // size in bytes
{
  if (size < 0 || size > 8)
    throw std::out_of_range("size must be 0..8");
  uint64_t res = 0;
  for (int i = 0; i < size; i++)
  {
    res <<= 8;
    res |= static_cast<unsigned char>(buffer[i]);
  }
  return res;
}

std::pair<uint64_t, int> ByteReader::read_varint(Database &db, uint64_t off)
{
  uint64_t v = 0;

  for (int i = 0; i < 8; i++)
  {
    char c;
    read_at(db.getDBFile(), &c, 1, off + i);
    uint8_t b = static_cast<uint8_t>(static_cast<unsigned char>(c));

    v = (v << 7) | (uint64_t)(b & 0x7F);
    if ((b & 0x80) == 0)
      return {v, i + 1};
  }

  char c9;
  read_at(db.getDBFile(), &c9, 1, off + 8);
  uint8_t b9 = static_cast<uint8_t>(static_cast<unsigned char>(c9));
  v = (v << 8) | (uint64_t)b9;
  return {v, 9};
}

int ByteReader::bytesForVarint(uint64_t v)
{
  if (v & (0xFFUL << 56))
    return 8;
  int c = 7;
  for (int i = 48; i >= 0; i -= 7, c--)
  {
    if (v & (0xFEUL << i))
      return c;
  }
  return 1;
}
