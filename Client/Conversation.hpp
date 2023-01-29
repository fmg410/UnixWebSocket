#ifndef CLIENT_CONVERSATION
#define CLIENT_CONVERSATION

#include <string>
#include <vector>
#include "Message.hpp"

class Conversation{
    const std::string person;
    std::vector<Message> messages;

public:
    Conversation(std::string p): person(p) {}
    const std::string getPerson() const { return person; }
    void addMessage(Message message);
    const std::vector<Message>& getMessageViewC() const;

};


#endif