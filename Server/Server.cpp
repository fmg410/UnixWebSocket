#include "Server.hpp"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <string>
#include <limits>
#include "../Client/Client.hpp"
#include "../Common.hpp"


void Server::addClient(const Client& c)
{
    clients.push_back(c);
}

void Server::removeClient(const std::string& name)
{
    auto itr = std::find_if(clients.begin(), clients.end(), [&](const Client& c){return c.name == name;});
    if(itr != clients.end())
        clients.erase(itr);
}

const Client& Server::getClientFromName(std::string& name)
{
    auto itr = std::find_if(clients.begin(), clients.end(), [&](const Client& c){return c.name == name;});
    if(itr == clients.end())
        throw 1;
    return *itr;
}

const Client& Server::getClientFromAddress(const sf::IpAddress& a, const unsigned short p)
{
    auto itr = std::find_if(clients.begin(), clients.end(), [&](const Client& c){return c.adress == a && c.port == p;});
    if(itr == clients.end())
        throw 1;
    return *itr;
}

bool Server::isClient(const std::string& name)
{
    auto itr = std::find_if(clients.begin(), clients.end(), [&](const Client& c){return c.name == name;});
    return itr != clients.end();
}