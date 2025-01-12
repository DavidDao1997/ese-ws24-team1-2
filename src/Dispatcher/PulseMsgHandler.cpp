/*
 * PulseMsgHandler.cpp
 *
 *  Created on: 02.12.2024
 *      Author: Marc
 */

#include "headers/PulseMsgHandler.h"

int32_t PulseMsgHandler::createChannel() {
    uint32_t channelID = -1;
    channelID = ChannelCreate(0);
    if (channelID < 0) {
        Logger::getInstance().log(LogLevel::CRITICAL, "Failed to create channel...", "PulseMsgHandler");
        throw std::runtime_error("Failed to create channel!");
    }
    return channelID;
}

name_attach_t *PulseMsgHandler::createNamedChannel(const std::string channelName) {
    Logger::getInstance().log(LogLevel::TRACE, "Creating GNS channel...", "PulseMsgHandler");
    name_attach_t *attach = name_attach(NULL, channelName.c_str(), NAME_FLAG_ATTACH_GLOBAL);
    if (attach == NULL) {
        Logger::getInstance().log(LogLevel::CRITICAL, "Failed to create GNS channel...", "PulseMsgHandler");
        // throw std::runtime_error("Failed to create GNS channel!");
    }
    Logger::getInstance().log(LogLevel::TRACE, "Created GNS channel...", "PulseMsgHandler");
    return attach;
}

int32_t PulseMsgHandler::connectToChannel(int32_t connectChannelID) {
    int connectionID = ConnectAttach(0, 0, connectChannelID, _NTO_SIDE_CHANNEL, 0);
    if (connectionID < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to connect for stop pulse...", "PulseMsgHandler");
    }
    return connectionID;
}

void PulseMsgHandler::destroyChannel(uint32_t channelID) {
    if (channelID >= 0) {
        int destroy_status = ChannelDestroy(channelID);
        if (destroy_status != EOK) {
            Logger::getInstance().log(LogLevel::ERROR, "Destroying channel failed...", "PulseMsgHandler");
        }
    }
}

void PulseMsgHandler::destroyNamedChannel(int32_t channelID, name_attach_t *attach) {
    int status = name_detach(attach, 0);
    if (status != EOK) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to unregister name in GNS...", "PulseMsgHandler");
        throw std::runtime_error("GNS deregistration failed");
    }
    // TODO WHY DOES DESTROY CHANNEL WONT WORK
    //destroyChannel(channelID);
}