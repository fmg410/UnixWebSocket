#ifndef CLIENT_MESSAGE
#define CLIENT_MESSAGE

#include <string>
#include <time.h>

struct Message{
    std::string contents{};
    bool sendBySelf{};
};


#endif