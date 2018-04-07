// License: Apache-2.0

#include "fabric/comm/connection.h"

namespace fabric::comm {

bool isTLSEnabled() {
    return false;
}

std::shared_ptr<grpc::ChannelCredentials> initTLSForShim(std::string key, std::string certStr) {
    // TODO
    return grpc::InsecureChannelCredentials();
}

std::shared_ptr<grpc::Channel> newClientConnectionWithAddress(std::string peerAddress, bool block, bool tlsEnabled,
                                                              const std::shared_ptr<grpc::ChannelCredentials>& creds,
                                                              const grpc::ChannelArguments& args) {
    // TODO
    return grpc::CreateCustomChannel(peerAddress, creds, args);
/*
func NewClientConnectionWithAddress(peerAddress string, block bool, tslEnabled bool,
        creds credentials.TransportCredentials, ka *KeepaliveOptions) (*grpc.ClientConn, error) {
        var opts []grpc.DialOption

        if ka != nil {
                opts = ClientKeepaliveOptions(ka)
        } else {
                // set to the default options
                opts = ClientKeepaliveOptions(DefaultKeepaliveOptions())
        }

        if tslEnabled {
                opts = append(opts, grpc.WithTransportCredentials(creds))
        } else {
                opts = append(opts, grpc.WithInsecure())
        }
        if block {
                opts = append(opts, grpc.WithBlock())
        }
        opts = append(opts, grpc.WithDefaultCallOptions(grpc.MaxCallRecvMsgSize(MaxRecvMsgSize()),
                grpc.MaxCallSendMsgSize(MaxSendMsgSize())))
        ctx := context.Background()
        ctx, _ = context.WithTimeout(ctx, defaultTimeout)
        conn, err := grpc.DialContext(ctx, peerAddress, opts...)
        if err != nil {
                return nil, err
        }
        return conn, err
}*/
    return 0;
}

}
