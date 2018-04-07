// License: Apache-2.0

#if !defined(FABRIC_SHIM_CHAINCODESTUB_H)
#define FABRIC_SHIM_CHAINCODESTUB_H

#include <string>
#include <vector>
#include "google/protobuf/timestamp.pb.h"
#include "peer/chaincode_event.pb.h"
#include "peer/proposal.pb.h"
#include "peer/proposal_response.pb.h"

namespace fabric::shim {

typedef std::uint8_t byte;

/// ChaincodeStub is an object passed to chaincode for shim side handling of
/// APIs.
class ChaincodeStub {
    std::string txid;

public:
    virtual ~ChaincodeStub() {}

    /// getArgs returns the arguments intended for the chaincode Init and Invoke
    /// as a vector of byte vectors.
    virtual std::vector<std::vector<byte>> getArgs() =0;

    /// getStringArgs returns the arguments intended for the chaincode Init and
    /// Invoke as a string array. Only use getStringArgs if the client passes
    /// arguments intended to be used as strings.
    virtual std::vector<std::string> getStringArgs();

    /// getFunctionAndParameters returns the first argument as the function
    /// name and the rest of the arguments as parameters in a string array.
    /// Only use getFunctionAndParameters if the client passes arguments intended
    /// to be used as strings.
    virtual void getFunctionAndParameters(std::string& func, std::vector<std::string>& params);

    /// GetArgsSlice returns the arguments intended for the chaincode Init and
    /// Invoke as a byte array
    virtual std::vector<byte> getArgsSlice() =0;

    /// getTxID returns the tx_id of the transaction proposal, which is unique per
    /// transaction and per client. See ChannelHeader in protos/common/common.proto
    /// for further details.
    virtual std::string getTxID() {
        return txid;
    }

    /// GetChannelID returns the channel the proposal is sent to for chaincode to process.
    /// This would be the channel_id of the transaction proposal (see ChannelHeader
    /// in protos/common/common.proto) except where the chaincode is calling another on
    /// a different channel
    virtual std::string getChannelID() =0;

    /// InvokeChaincode locally calls the specified chaincode `Invoke` using the
    /// same transaction context; that is, chaincode calling chaincode doesn't
    /// create a new transaction message.
    /// If the called chaincode is on the same channel, it simply adds the called
    /// chaincode read set and write set to the calling transaction.
    /// If the called chaincode is on a different channel,
    /// only the Response is returned to the calling chaincode; any PutState calls
    /// from the called chaincode will not have any effect on the ledger; that is,
    /// the called chaincode on a different channel will not have its read set
    /// and write set applied to the transaction. Only the calling chaincode's
    /// read set and write set will be applied to the transaction. Effectively
    /// the called chaincode on a different channel is a `Query`, which does not
    /// participate in state validation checks in subsequent commit phase.
    /// If `channel` is empty, the caller's channel is assumed.
    virtual protos::Response invokeChaincode(std::string chaincodeName, std::vector<std::vector<byte>> args, std::string channel) =0;

    /// getState returns the value of the specified `key` from the
    /// ledger. Note that getState doesn't read data from the writeset, which
    /// has not been committed to the ledger. In other words, getState doesn't
    /// consider data modified by putState that has not been committed.
    /// If the key does not exist in the state database, (nil, nil) is returned.
    virtual std::vector<byte> getState(std::string key) =0;
    virtual std::string getStateAsString(std::string key) =0;

    /// putState puts the specified `key` and `value` into the transaction's
    /// writeset as a data-write proposal. putState doesn't effect the ledger
    /// until the transaction is validated and successfully committed.
    /// Simple keys must not be an empty string and must not start with null
    /// character (0x00), in order to avoid range query collisions with
    /// composite keys, which internally get prefixed with 0x00 as composite
    /// key namespace.
    virtual void putState(std::string key, const std::vector<byte>& value) =0;
    virtual void putState(std::string key, const std::string& value) =0;

    /// delState records the specified `key` to be deleted in the writeset of
    /// the transaction proposal. The `key` and its value will be deleted from
    /// the ledger when the transaction is validated and successfully committed.
    virtual void delState(std::string key) =0;

    /// getStateByRange returns a range iterator over a set of keys in the
    /// ledger. The iterator can be used to iterate over all keys
    /// between the startKey (inclusive) and endKey (exclusive).
    /// The keys are returned by the iterator in lexical order. Note
    /// that startKey and endKey can be empty string, which implies unbounded range
    /// query on start or end.
    /// Call close() on the returned StateQueryIteratorInterface object when done.
    /// The query is re-executed during validation phase to ensure result set
    /// has not changed since transaction endorsement (phantom reads detected).
    virtual void /*StateQueryIteratorInterface*/ getStateByRange(std::string startKey, std::string endKey) =0;

    /// getStateByPartialCompositeKey queries the state in the ledger based on
    /// a given partial composite key. This function returns an iterator
    /// which can be used to iterate over all composite keys whose prefix matches
    /// the given partial composite key. The `objectType` and attributes are
    /// expected to have only valid utf8 strings and should not contain
    /// U+0000 (nil byte) and U+10FFFF (biggest and unallocated code point).
    /// See related functions splitCompositeKey and CreateCompositeKey.
    /// Call close() on the returned StateQueryIteratorInterface object when done.
    /// The query is re-executed during validation phase to ensure result set
    /// has not changed since transaction endorsement (phantom reads detected).
    virtual void /*StateQueryIteratorInterface*/ getStateByPartialCompositeKey(std::string objectType, std::vector<std::string> keys) =0;

    /// createCompositeKey combines the given `attributes` to form a composite
    /// key. The objectType and attributes are expected to have only valid utf8
    /// strings and should not contain U+0000 (nil byte) and U+10FFFF
    /// (biggest and unallocated code point).
    /// The resulting composite key can be used as the key in putState().
    virtual std::string createCompositeKey(std::string objectType, std::vector<std::string> attributes) =0;

    /// splitCompositeKey splits the specified key into attributes on which the
    /// composite key was formed. Composite keys found during range queries
    /// or partial composite key queries can therefore be split into their
    /// composite parts.
    virtual void /*string, []string, error*/ splitCompositeKey(std::string compositeKey) =0; // TODO

    /// getQueryResult performs a "rich" query against a state database. It is
    /// only supported for state databases that support rich query,
    /// e.g.CouchDB. The query string is in the native syntax
    /// of the underlying state database. An iterator is returned
    /// which can be used to iterate (next) over the query result set.
    /// The query is NOT re-executed during validation phase, phantom reads are
    /// not detected. That is, other committed transactions may have added,
    /// updated, or removed keys that impact the result set, and this would not
    /// be detected at validation/commit time.  Applications susceptible to this
    /// should therefore not use GetQueryResult as part of transactions that update
    /// ledger, and should limit use to read-only chaincode operations.
    virtual void /*StateQueryIteratorInterface*/ getQueryResult(std::string query);

    /// getHistoryForKey returns a history of key values across time.
    /// For each historic key update, the historic value and associated
    /// transaction id and timestamp are returned. The timestamp is the
    /// timestamp provided by the client in the proposal header.
    /// GetHistoryForKey requires peer configuration
    /// core.ledger.history.enableHistoryDatabase to be true.
    /// The query is NOT re-executed during validation phase, phantom reads are
    /// not detected. That is, other committed transactions may have updated
    /// the key concurrently, impacting the result set, and this would not be
    /// detected at validation/commit time. Applications susceptible to this
    /// should therefore not use GetHistoryForKey as part of transactions that
    /// update ledger, and should limit use to read-only chaincode operations.
    virtual void /*HistoryQueryIteratorInterface*/ getHistoryForKey(std::string key);

    /// getCreator returns `SignatureHeader.Creator` (e.g. an identity)
    /// of the `SignedProposal`. This is the identity of the agent (or user)
    /// submitting the transaction.
    virtual std::vector<byte> getCreator();

    /// getTransient returns the `ChaincodeProposalPayload.Transient` field.
    /// It is a map that contains data (e.g. cryptographic material)
    /// that might be used to implement some form of application-level
    /// confidentiality. The contents of this field, as prescribed by
    /// `ChaincodeProposalPayload`, are supposed to always
    /// be omitted from the transaction and excluded from the ledger.
    virtual std::map<std::string, std::vector<byte>> getTransient() =0;

    /// getBinding returns the transaction binding, which is used to enforce a
    /// link between application data (like those stored in the transient field
    /// above) to the proposal itself. This is useful to avoid possible replay
    /// attacks.
    virtual std::vector<byte> getBinding();

    /// getDecorations returns additional data (if applicable) about the proposal
    /// that originated from the peer. This data is set by the decorators of the
    /// peer, which append or mutate the chaincode input passed to the chaincode.
    virtual std::map<std::string, std::vector<byte>> getDecorations() =0;

    /// getSignedProposal returns the SignedProposal object, which contains all
    /// data elements part of a transaction proposal.
    virtual protos::SignedProposal getSignedProposal();

    /// getTxTimestamp returns the timestamp when the transaction was created. This
    /// is taken from the transaction ChannelHeader, therefore it will indicate the
    /// client's timestamp and will have the same value across all endorsers.
    virtual google::protobuf::Timestamp getTxTimestamp();

    /// setEvent allows the chaincode to propose an event on the transaction
    /// proposal. If the transaction is validated and successfully committed,
    /// the event will be delivered to the current event listeners.
    virtual void setEvent(std::string name, std::vector<byte> payload) =0;

    virtual protos::ChaincodeEvent getEvent() =0;

/*
type ChaincodeStub struct {
        TxID           string
        chaincodeEvent *pb.ChaincodeEvent
        args           [][]byte
        handler        *Handler
        signedProposal *pb.SignedProposal
        proposal       *pb.Proposal

        // Additional fields extracted from the signedProposal
        creator   []byte
        transient map[string][]byte
        binding   []byte

        decorations map[string][]byte
}

*/


};

}

#endif // !defined(FABRIC_SHIM_CHAINCODESTUB_H)
