#include "Database.hpp"

Database::Database(const std::string &database_file_path)
    : database_file(database_file_path, std::ios::binary)
{
  if (!database_file)
  {
    throw std::runtime_error("Failed to open database file: " + database_file_path);
  }
}

void Database::setDbInfo()
{
  char buffer[2];
  Br.read_at(database_file, buffer, 2, 16);
  page_size = Br.byteToU16(buffer);

  Br.read_at(database_file, buffer, 2, 103);
  tables = Br.byteToU16(buffer);
}

void Database::getDbInfo()
{
  setDbInfo();
  std::cout << "database page size: " << page_size << std::endl;
  std::cout << "number of tables: " << tables << std::endl;
}

std::ifstream& Database::getDBFile(){
  return database_file;
}