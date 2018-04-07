// License: Apache-2.0

#if !defined(FABRIC_SHIM_HANDLER_H)
#define FABRIC_SHIM_HANDLER_H

#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "grpc/grpc.h"
#include "grpcpp/channel.h"
#include "grpcpp/support/sync_stream.h"

#include "peer/chaincode_shim.pb.h"
#include "channel.h"
#include "fsm.h"

namespace fabric::shim {

typedef ::grpc::ClientReaderWriter< ::protos::ChaincodeMessage, ::protos::ChaincodeMessage> PeerChaincodeStream;

enum States {
    Created,
    Established,
    Ready,
    Init
};

class Handler {
    typedef FSM<protos::ChaincodeMessage, States> ChaincodeFSM;

    //std::unique_ptr<PeerChaincodeStream> chatStream;
    PeerChaincodeStream* chatStream;
    ChaincodeFSM* fsm;
    std::map<std::string, Channel<protos::ChaincodeMessage>*> responseChannel;

    std::mutex sync;

    void initFSM();

    Handler() {
        //chatStream = Ch
    }

public:
   static Handler* newChaincodeHandler();

    void triggerNextState(bool send);
    void serialSend(protos::ChaincodeMessage& msg) {
        std::unique_lock<std::mutex> lock(sync);
        chatStream->Write(msg);
    }
    void serialSendAsync();
    std::string getTxCtxId(std::string chainID, std::string txid) {
        return chainID + txid;
    }

    Channel<protos::ChaincodeMessage>* createChannel(std::string channelID, std::string txid);
    void sendChannel(protos::ChaincodeMessage& msg);
    protos::ChaincodeMessage sendReceive(protos::ChaincodeMessage& msg);
    void deleteChannel(std::string channelID, std::string txid);

    void beforeRegistered(ChaincodeFSM::EventData& evt);
    void afterResponse(ChaincodeFSM::EventData& evt);
    void afterError(ChaincodeFSM::EventData& evt);
    void beforeInit(ChaincodeFSM::EventData& evt);
    void beforeTransaction(ChaincodeFSM::EventData& evt);

    void handleInit(ChaincodeFSM::EventData& evt);
};

}

#endif // !defined(FABRIC_SHIM_HANDLER_H)

