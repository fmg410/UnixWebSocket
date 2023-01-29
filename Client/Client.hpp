#ifndef CLIENT_CLIENT
#define CLIENT_CLIENT


#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <string>
#include <limits>
#include "../Common.hpp"


struct Client
{
    std::string name;
    std::string password;
    const sf::IpAddress adress;
    const unsigned short port;
    void runUdpClient(unsigned short clientPort, sf::IpAddress serverIp, unsigned short serverPort);
    bool operator==(const Client &client)
    {
        return adress == client.adress && port == client.port;
    }
    bool operator!=(const Client &client)
    {
        return !(*this == client);
    }
};







#endif