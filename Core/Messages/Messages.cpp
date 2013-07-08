#include "Messages.h"

#include <map>


MessageDispatcher gMessageDispatcher;


void MessageDispatcher::registerHandler( Message::EType messageType
	                                   , MessageHandler* messageHandler )
{
	handlers.insert(std::make_pair(messageType, messageHandler));
}
