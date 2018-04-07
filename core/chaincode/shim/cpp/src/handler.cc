// License: Apache-2.0

#include "handler.h"

namespace fabric::shim {

void Handler::initFSM() {
    const ChaincodeFSM::EventDesc eventDesc[] = {
        ChaincodeFSM::EventDesc(protos::ChaincodeMessage::REGISTERED, Created, Established),
        ChaincodeFSM::EventDesc(protos::ChaincodeMessage::READY, Established, Ready),
        ChaincodeFSM::EventDesc(protos::ChaincodeMessage::ERROR, Init, Established),
        ChaincodeFSM::EventDesc(protos::ChaincodeMessage::RESPONSE, Init, Init),
        ChaincodeFSM::EventDesc(protos::ChaincodeMessage::INIT, Ready, Ready),
        ChaincodeFSM::EventDesc(protos::ChaincodeMessage::TRANSACTION, Ready, Ready),
        ChaincodeFSM::EventDesc(protos::ChaincodeMessage::RESPONSE, Ready, Ready),
        ChaincodeFSM::EventDesc(protos::ChaincodeMessage::COMPLETED, Init, Ready),
        ChaincodeFSM::EventDesc(protos::ChaincodeMessage::COMPLETED, Ready, Ready),
    };
    const size_t eventDescCnt = sizeof(eventDesc) / sizeof(ChaincodeFSM::EventDesc);
    const ChaincodeFSM::Callback callbacks[] = {
        ChaincodeFSM::Callback(ChaincodeFSM::Before, protos::ChaincodeMessage::REGISTERED, std::bind(&Handler::beforeRegistered, this, std::placeholders::_1)),
        ChaincodeFSM::Callback(ChaincodeFSM::After, protos::ChaincodeMessage::RESPONSE, std::bind(&Handler::afterResponse, this, std::placeholders::_1)),
        ChaincodeFSM::Callback(ChaincodeFSM::After, protos::ChaincodeMessage::ERROR, std::bind(&Handler::afterError, this, std::placeholders::_1)),
        ChaincodeFSM::Callback(ChaincodeFSM::Before, protos::ChaincodeMessage::INIT, std::bind(&Handler::beforeInit, this, std::placeholders::_1)),
        ChaincodeFSM::Callback(ChaincodeFSM::Before, protos::ChaincodeMessage::TRANSACTION, std::bind(&Handler::beforeTransaction, this, std::placeholders::_1)),
    };
    const size_t callbacksCnt = sizeof(callbacks) / sizeof(ChaincodeFSM::Callback);
    fsm = new ChaincodeFSM(Created, eventDesc, eventDescCnt, callbacks, callbacksCnt);
}

Handler* Handler::newChaincodeHandler() {
    auto handler = new Handler();
    handler->initFSM();
    return handler;
}

Channel<protos::ChaincodeMessage>* Handler::createChannel(std::string channelID, std::string txid) {
    std::lock_guard<std::mutex> lockState(sync);
    auto txCtxId = getTxCtxId(channelID, txid);
    if (responseChannel.count(txCtxId) == 0) {
        // Create channel, not message!!!
        auto channel = new Channel<protos::ChaincodeMessage>();
        responseChannel[txCtxId] = channel;
        return channel;
    }
    else {
        throw std::logic_error("Channel already created");
    }
}

void Handler::sendChannel(protos::ChaincodeMessage& msg) {
    std::lock_guard<std::mutex> lockState(sync);
    auto txCtxId = getTxCtxId(msg.channel_id(), msg.txid());
    auto iter = responseChannel.find(txCtxId);
    if (iter != responseChannel.end()) {
        auto channel = iter->second;
        channel->put(msg);
    }
    else {
        throw std::logic_error("Channel does not exist");
    }
}

protos::ChaincodeMessage Handler::sendReceive(protos::ChaincodeMessage& msg) {
    std::lock_guard<std::mutex> lockState(sync);
    return protos::ChaincodeMessage();
}

void Handler::deleteChannel(std::string channelID, std::string txid) {
    std::lock_guard<std::mutex> lockState(sync);
    auto txCtxId = getTxCtxId(channelID, txid);
    auto iter = responseChannel.find(txCtxId);
    if (iter != responseChannel.end()) {
        auto channel = iter->second;
        responseChannel.erase(iter);
        delete channel;
    }
}

void Handler::beforeRegistered(ChaincodeFSM::EventData& evt) {
}

void Handler::afterResponse(ChaincodeFSM::EventData& evt) {
}

void Handler::afterError(ChaincodeFSM::EventData& evt) {
}

void Handler::beforeInit(ChaincodeFSM::EventData& evt) {
}

void Handler::beforeTransaction(ChaincodeFSM::EventData& evt) {
}


}
