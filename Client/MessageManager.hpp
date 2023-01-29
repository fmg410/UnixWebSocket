#ifndef CLIENT_MESSAGEMANAGER
#define CLIENT_MESSAGEMANAGER

#include <vector>
#include <string>
#include "Message.hpp"
#include "Conversation.hpp"

class MessageManager{
private:
    std::vector<Conversation> conversations;
    void addMessage(std::string person, std::string contents, bool sentBySelf);

public:
    void receivedMessage(std::string person, std::string contents);
    void sentMessage(std::string person, std::string contents);
    const std::vector<Conversation>& getConversationViewC();
    const std::vector<Message>& getMessageViewC(std::string person);
    bool isConversation(std::string person);
};



#endif