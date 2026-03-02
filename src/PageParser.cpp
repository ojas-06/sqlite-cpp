#include "./include/PageParser.hpp"

PageParser::PageParser(Database &db, unsigned int page_no) : page_no(page_no), db(db)
{
  page_offset = db.page_size * (page_no - 1);
  unsigned int pre_off = page_offset + ((page_no == 1) ? 100 : 0);
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
      // std::cout << stc << std::endl;
      curr_offset += off;
    }
    // for (auto x : serialTypeCodes)
    //   std::cout << x << std::endl;

    // skipping first two columns
    // columns in schema are : (type, name, tbl_name, rootpage, sql)

    for (int i = 0; i < 2; i++)
    {
      if (serialTypeCodes[i] <= 4)
        curr_offset += serialTypeCodes[i];
      if (serialTypeCodes[i] == 5)
        curr_offset += 6;
      if (serialTypeCodes[i] == 6 || serialTypeCodes[i] == 7)
        curr_offset += 8;
      if (!(serialTypeCodes[i] % 2) && (serialTypeCodes[i] >= 12))
        curr_offset += (serialTypeCodes[i] - 12) / 2;
      if ((serialTypeCodes[i] % 2) && (serialTypeCodes[i] >= 13))
        curr_offset += (serialTypeCodes[i] - 13) / 2;
    }
    // std::cout << serialTypeCodes[2] << std::endl;
    if ((serialTypeCodes[2] % 2) && (serialTypeCodes[2] >= 13))
    {
      // tbl_name == string
      // std::cout << "string\n";
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
