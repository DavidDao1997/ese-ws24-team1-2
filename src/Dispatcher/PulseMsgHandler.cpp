/*
 * PulseMsgHandler.cpp
 *
 *  Created on: 02.12.2024
 *      Author: Marc
 */


#include "header/PulseMsgHandler.h"


int32_t PulseMsgHandler::createChannel(){
    uint32_t channelID = -1;
    channelID = ChannelCreate(0);
    if (channelID < 0) {
        throw std::runtime_error("Failed to create channel!");
    }
    return channelID;
}


name_attach_t* PulseMsgHandler::createNamedChannel(const std::string channelName){
    name_attach_t* attach = name_attach(NULL, channelName, 0);
    if (attach == NULL){
        throw std::runtime_error("Failed to create GNS channel!");
    }
    return attach;
}

int32_t PulseMsgHandler::connectToChannel(int32_t connectChannelID){
    int connectionID = ConnectAttach(0, 0, connectChannelID, _NTO_SIDE_CHANNEL, 0);
    if (connectionID < 0) {
        perror("Failed to connect for stop pulse!");
    }
    return connectionID;
}

void PulseMsgHandler::destroyChannel(uint32_t channelID){
    if (channelID >= 0) {
        int destroy_status = ChannelDestroy(channelID);
        if (destroy_status != EOK) {
            perror("Destroying channel failed!");
        }
    }
}

void PulseMsgHandler::destroyNamedChannel(int32_t channelID, name_attach_t* attach){
    int status = name_detach(attach, 0);
    if (status != EOK) {
        perror("Failed to unregister name in GNS");
        throw std::runtime_error("GNS deregistration failed");
    }
    destroyChannel(channelID);
}