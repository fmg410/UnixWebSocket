#ifndef AUTHENTICATOR
#define AUTHENTICATOR

#include <SFML/Network.hpp>
#include "Common.hpp"

bool validatePacket(sf::Packet& packet);
void preparePacket(sf::Packet& packet);

#endif