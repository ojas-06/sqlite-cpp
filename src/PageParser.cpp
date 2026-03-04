#include "./include/PageParser.hpp"

uint16_t PageParser::getCellCount()
{
  return this->cellCount;
}

PageParser::PageParser(Database &db, uint32_t page_no) : page_no(page_no), db(db)
{
  page_offset = db.page_size * (page_no - 1);
  uint64_t pre_off = page_offset + ((page_no == 1) ? 100 : 0);
  char c;
  Br.read_at(db.getDBFile(), &c, 1, 0, pre_off);
  uint8_t pageType = static_cast<uint8_t>(c);
  switch (pageType)
  {
  case 2:
    page_type = {true, false};
    break;
  case 5:
    page_type = {true, true};
    break;
  case 10:
    page_type = {false, false};
    break;
  case 13:
    page_type = {false, true};
    break;
  default:
    std::cerr << "Page header Error - Page type not matching\n";
    exit(1);
  }
  char buffer[2];
  Br.read_at(db.getDBFile(), buffer, 2, 3, pre_off);
  cellCount = Br.byteToU16(buffer);

  for (size_t i = 0; i < cellCount; i++)
  {
    Br.read_at(db.getDBFile(), buffer, 2, (page_type[0] ? 12 : 8) + 2 * i, pre_off);
    cellPointers.push_back(Br.byteToU16(buffer));
  }
}

uint64_t PageParser::sizeBySerialType(uint64_t stc)
{
  if (stc <= 4)
    return stc;
  if (stc == 5)
    return 6;
  if (stc == 6 || stc == 7)
    return 8;
  if (!(stc % 2) && (stc >= 12))
    return (stc - 12) / 2;
  if ((stc % 2) && (stc >= 13))
    return (stc - 13) / 2;
  return 0;
}

void PageParser::printUserTableNames() // schema table cell is a table btree leaf cell
{
  for (auto cell_offset : cellPointers) // reading one cell
  {
    int curr_offset = cell_offset + page_offset, off;
    uint64_t recordSize, rowid;
    std::tie(recordSize, off) = Br.read_varint(db, curr_offset);
    curr_offset += off;
    std::tie(rowid, off) = Br.read_varint(db, curr_offset);
    curr_offset += off;

    uint64_t recordHeaderSize;
    std::tie(recordHeaderSize, off) = Br.read_varint(db, curr_offset);
    int bytesForHeaderSize = Br.bytesForVarint(recordHeaderSize);
    curr_offset += bytesForHeaderSize;

    uint64_t stc;
    std::vector<uint64_t> serialTypeCodes;
    for (int i = 0; i < 5; i++)
    {
      std::tie(stc, off) = Br.read_varint(db, curr_offset);
      serialTypeCodes.push_back(stc);
      curr_offset += off;
    }

    // skipping first two columns
    // columns in schema are : (type, name, tbl_name, rootpage, sql)

    for (int i = 0; i < 2; i++)
    {
      curr_offset += sizeBySerialType(serialTypeCodes[i]);
    }
    if ((serialTypeCodes[2] % 2) && (serialTypeCodes[2] >= 13))
    {
      // tbl_name == string
      unsigned int stringSize = (serialTypeCodes[2] - 13) / 2;
      char *buffer;
      buffer = (char *)malloc(stringSize);
      Br.read_at(db.getDBFile(), buffer, stringSize, static_cast<unsigned long int>(curr_offset));
      for (int i = 0; i < (int)stringSize; i++)
        std::cout << buffer[i];
      std::cout << " ";
      free(buffer);
    }
  }
  std::cout << std::endl;
}

uint32_t PageParser::pageNoByTblName(std::string &tblName)
{
  for (auto cell_offset : cellPointers)
  {
    int curr_offset = cell_offset + page_offset, off;
    uint64_t recordSize, rowid;
    std::tie(recordSize, off) = Br.read_varint(db, curr_offset);
    curr_offset += off;
    std::tie(rowid, off) = Br.read_varint(db, curr_offset);
    curr_offset += off;

    uint64_t recordHeaderSize;
    std::tie(recordHeaderSize, off) = Br.read_varint(db, curr_offset);
    int bytesForHeaderSize = Br.bytesForVarint(recordHeaderSize);
    curr_offset += bytesForHeaderSize;

    uint64_t stc;
    std::vector<uint64_t> serialTypeCodes;
    for (int i = 0; i < 5; i++)
    {
      std::tie(stc, off) = Br.read_varint(db, curr_offset);
      serialTypeCodes.push_back(stc);
      curr_offset += off;
    }

    // skipping first two columns
    // columns in schema are : (type, name, tbl_name, rootpage, sql)

    for (int i = 0; i < 2; i++)
    {
      curr_offset += sizeBySerialType(serialTypeCodes[i]);
    }
    if ((serialTypeCodes[2] % 2) && (serialTypeCodes[2] >= 13))
    {
      // tbl_name == string
      unsigned int stringSize = (serialTypeCodes[2] - 13) / 2;
      char *buffer;
      buffer = (char *)malloc(stringSize + 1);
      Br.read_at(db.getDBFile(), buffer, stringSize, static_cast<unsigned long int>(curr_offset));
      buffer[stringSize] = '\0';
      if (!strcmp(buffer, tblName.data()))
      {
        curr_offset += stringSize;
        uint64_t rootpage;
        uint64_t rootPgNoSize = sizeBySerialType(serialTypeCodes[3]);
        buffer = (char *)realloc(buffer, rootPgNoSize);
        Br.read_at(db.getDBFile(), buffer, rootPgNoSize, static_cast<unsigned long int>(curr_offset));
        rootpage = Br.byteToU64(buffer, rootPgNoSize);

        free(buffer);
        return rootpage;
      }
      free(buffer);
    }
  }
  return 0;
}
