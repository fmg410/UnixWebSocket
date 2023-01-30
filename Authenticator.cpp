#include "Authenticator.hpp"


bool validatePacket(sf::Packet& packet)
{
    unsigned int id = 0;

    packet >> id;

    return packet && id == appId_;

}

void preparePacket(sf::Packet& packet)
{
    packet << appId_;
}