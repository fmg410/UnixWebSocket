#ifndef CLIENT_RUNCLIENT
#define CLIENT_RUNCLIENT

#include <SFML/Network.hpp>
#include "../Common.hpp"

void runUdpClient(unsigned short clientPort, sf::IpAddress serverIp, unsigned short serverPort);

#endif