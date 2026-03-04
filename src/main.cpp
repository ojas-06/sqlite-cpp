#include "./include/Database.hpp"
#include "./include/PageParser.hpp"

std::vector<std::string> split(std::string &s, char delim)
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
    return splitted;
}

int main(int argc, char *argv[])
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cerr << "Logs from your program will appear here" << std::endl;

    if (argc != 3)
    {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string database_file_path = argv[1];
    std::string command = argv[2];

    Database Db(database_file_path);

    if (command == ".dbinfo")
    {
        Db.getDbInfo();
    }
    else if (command == ".tables")
    {
        PageParser schemaParser(Db, 1);
        schemaParser.printUserTableNames();
    }
    else
    {
        std::vector<std::string> splitted = split(command, ' ');
        std::string tableName = splitted.back();
        PageParser schemaParser(Db, 1);
        uint32_t rootpage = schemaParser.pageNoByTblName(tableName);
        PageParser P(Db, rootpage);
        std::cout << P.getCellCount() << std::endl;
    }

    return 0;
}
