#ifndef SERVER_SERVER
#define SERVER_SERVER

#include <string>
#include <vector>
#include <SFML/Network.hpp>
#include "../Common.hpp"

class Client;

class Server{
    const unsigned short port;
    std::vector<Client> clients;
public:
    Server(const unsigned short p) : port(p) {}
    unsigned short getPort() { return port; }
    unsigned int size() { return clients.size(); }
    const std::vector<Client>& getClientViewC() { return clients; };
    void addClient(const Client& c);
    void removeClient(const std::string& name);
    const Client& getClientFromName(std::string& name);
    const Client& getClientFromAddress(const sf::IpAddress& a, const unsigned short p);
    bool isClient(const std::string& name);
};



#endif