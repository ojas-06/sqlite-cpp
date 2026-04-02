#include "./include/Page_cmdParser.hpp"

CommandParser::CommandParser(Database &db, std::string cmd) : db(db), command(cmd)
{
  splitted = split(cmd, ' ');
  query = splitted[0];
}

std::vector<std::string> CommandParser::split(std::string &s, char delim)
{
  std::vector<std::string> splitted = {""};
  int top = 0;
  for (char c : s)
  {
    if (c == delim)
    {
      splitted.push_back("");
      top++;
    }
    else
      splitted[top] += c;
  }
  std::transform(splitted[0].begin(), splitted[0].end(), splitted[0].begin(),
                 [](unsigned char c)
                 { return std::toupper(c); });
  return splitted;
}

void CommandParser::taskByQuery()
{
  if (query == "SELECT")
  {
    if (splitted[1] == "COUNT(*)")
    {
      std::string tableName = splitted.back();
      if (tableName.back() == ';')
        tableName = tableName.substr(0, tableName.size() - 1);
      PageParser schemaParser(db, 1);
      uint32_t rootpage = schemaParser.pageNoByTblName(tableName);
      PageParser P(db, rootpage);
      std::cout << P.getCellCount() << std::endl;
    }
    else
    {
      std::string tableName = splitted.back();
      std::string column = splitted[1];

      PageParser schemaParser(db, 1);
      std::vector<std::string> columns;
      uint32_t rootpage = schemaParser.columnNames_pageNo(tableName, columns);
      PageParser P(db, rootpage,columns);
    }
  }
}

void CommandParser::getColumnsFromCreateTable(std::vector<std::string> &columns)
{
  size_t i = command.find('(');
  size_t j = command.find(')');
  std::string temp = command.substr(i + 1, j);
  splitted.clear();
  splitted = split(temp, ',');
  for (std::string &s : splitted)
  {
    std::vector<std::string> t = split(s, ' ');
    columns.push_back(t[0]);
  }
}
