#include <iostream>

#include "Server/RunServer.hpp"
#include "Client/RunClient.hpp"





int main(int argc, char *argv[])
{
    const int serverPort = 50001;

    if (argc == 3)
    {
        bool success = true;
        sf::IpAddress address(argv[1]);
        if (address == sf::IpAddress::None)
            success = false;

        unsigned short port = 0;
        try
        {
            port = std::stoi(std::string(argv[2]));
        }
        catch (...)
        {
            success = false;
        }

        if (success)
            runUdpClient(port, address, serverPort);
    }

    std::cout << "c - client ; s - server ; empty - server" << std::endl;
    char choice;
    std::cin >> choice;
    std::cin.ignore(10000, '\n');

    if (choice == 'c')
        runUdpClient(sf::Socket::AnyPort, sf::IpAddress::getLocalAddress(), serverPort);
    else if (choice == 's')
        runUdpServer(serverPort);
    else
        runUdpClient(sf::Socket::AnyPort, sf::IpAddress::getLocalAddress(), serverPort);

    return 0;
}