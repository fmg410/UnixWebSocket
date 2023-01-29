#include "Conversation.hpp"

void Conversation::addMessage(Message message)
{
    messages.push_back(message);
}

const std::vector<Message>& Conversation::getMessageViewC() const
{
    return messages;
}