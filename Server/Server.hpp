#ifndef SERVER_SERVER
#define SERVER_SERVER

#include <vector>
#include "../Common.hpp"

class Client;

class Server{
    const unsigned short port;
    std::vector<Client> clients;
};



#endif