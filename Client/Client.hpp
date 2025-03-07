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
    std::string name{};
    std::string password{};
    sf::IpAddress adress{};
    unsigned short port{};

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