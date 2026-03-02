#include "./include/Database.hpp"
#include "./include/PageParser.hpp"

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

    return 0;
}
