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

void PageParser::printUserTableNames()
{
  for (auto cell_offset : cellPointers)
  {
    int curr_offset = cell_offset + page_offset;
    std::pair<uint64_t, int> recordSize = Br.read_varint(db, curr_offset);
    curr_offset += recordSize.second;
    std::pair<uint64_t, int> rowid = Br.read_varint(db, curr_offset);
    curr_offset += rowid.second;

    char *payload;
    payload = (char *)malloc(recordSize.first);
    Br.read_at(db.getDBFile(), payload, recordSize.first, curr_offset);

    // Pending -> make sense of payload header and body

    free(payload);
  }
}
