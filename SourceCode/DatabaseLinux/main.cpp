#include <iostream>
#include "src/MySQLDatabase.h"



int main(int argc, char *argv[])
{
    const std::string DBHost("localhost");
    const std::string DBUser("root");
    const std::string DBPassword("root");
    const std::string DBName("test");
    const int DBPort = 3306;

    MySQLDatabase MySQL;
    if (!MySQL.Connect(DBHost.c_str(), DBUser.c_str(), DBPassword.c_str(), DBName.c_str(), DBPort))
    {
        std::cout << "Connect mysql failed." << std::endl;
    }
    else
    {
        std::cout << "Connect mysql succefully." << std::endl;
    }

    int x = 0;
    std::cin >> x;

    return 0;
}
