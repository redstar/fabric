
// Written in the D programming language.
//===-- chaincode.d - Chaincodeinterface ----------*- D -*-===//
//
//                     Hyperledger Fabric
//
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief
/// This file contains the chaincode implementation.
///
//===----------------------------------------------------------------------===//

module fabric.shim.chaincode;

public import google.protobuf;
public import protos;

struct SmartContract
{
}

interface Chaincode
{
    /// Init is called during Instantiate transaction after the chaincode container
    /// has been established for the first time, allowing the chaincode to
    /// initialize its internal data
    /// @param stub
    Response init(ChaincodeStub stub);

    /// Invoke is called to update or query the ledger in a proposal transaction.
    /// Updated state variables are not committed to the ledger until the
    /// transaction is committed.
    /// @param stub
    Response invoke(ChaincodeStub stub);
}

interface ChaincodeStub
{
    /// getArgs returns the arguments intended for the chaincode Init and Invoke
    /// as a vector of byte vectors.
    byte[][] getArgs();

    /// getStringArgs returns the arguments intended for the chaincode Init and
    /// Invoke as a string array. Only use getStringArgs if the client passes
    /// arguments intended to be used as strings.
    string[] getStringArgs();

    /// getFunctionAndParameters returns the first argument as the function
    /// name and the rest of the arguments as parameters in a string array.
    /// Only use getFunctionAndParameters if the client passes arguments intended
    /// to be used as strings.
    void getFunctionAndParameters(ref string func, ref string[] params);

    /// GetArgsSlice returns the arguments intended for the chaincode Init and
    /// Invoke as a byte array
    byte[] getArgsSlice();

    /// getTxID returns the tx_id of the transaction proposal, which is unique per
    /// transaction and per client. See ChannelHeader in protos/common/common.proto
    /// for further details.
    string getTxID();

    /// GetChannelID returns the channel the proposal is sent to for chaincode to process.
    /// This would be the channel_id of the transaction proposal (see ChannelHeader
    /// in protos/common/common.proto) except where the chaincode is calling another on
    /// a different channel
    string getChannelID();

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
    Response invokeChaincode(string chaincodeName, byte[][] args, string channel);

    /// getState returns the value of the specified `key` from the
    /// ledger. Note that getState doesn't read data from the writeset, which
    /// has not been committed to the ledger. In other words, getState doesn't
    /// consider data modified by putState that has not been committed.
    /// If the key does not exist in the state database, (nil, nil) is returned.
    byte[] getState(string key);
    string getStateAsString(string key);

    /// putState puts the specified `key` and `value` into the transaction's
    /// writeset as a data-write proposal. putState doesn't effect the ledger
    /// until the transaction is validated and successfully committed.
    /// Simple keys must not be an empty string and must not start with null
    /// character (0x00), in order to avoid range query collisions with
    /// composite keys, which internally get prefixed with 0x00 as composite
    /// key namespace.
    void putState(string key, const byte[] value);
    void putState(string key, const string[] value);

    /// delState records the specified `key` to be deleted in the writeset of
    /// the transaction proposal. The `key` and its value will be deleted from
    /// the ledger when the transaction is validated and successfully committed.
    void delState(string key);

    /// getStateByRange returns a range iterator over a set of keys in the
    /// ledger. The iterator can be used to iterate over all keys
    /// between the startKey (inclusive) and endKey (exclusive).
    /// The keys are returned by the iterator in lexical order. Note
    /// that startKey and endKey can be empty string, which implies unbounded range
    /// query on start or end.
    /// Call close() on the returned StateQueryIteratorInterface object when done.
    /// The query is re-executed during validation phase to ensure result set
    /// has not changed since transaction endorsement (phantom reads detected).
    void /*StateQueryIteratorInterface*/ getStateByRange(string startKey, string endKey);

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
    void /*StateQueryIteratorInterface*/ getStateByPartialCompositeKey(string objectType, string[] keys);

    /// createCompositeKey combines the given `attributes` to form a composite
    /// key. The objectType and attributes are expected to have only valid utf8
    /// strings and should not contain U+0000 (nil byte) and U+10FFFF
    /// (biggest and unallocated code point).
    /// The resulting composite key can be used as the key in putState().
    string createCompositeKey(string objectType, string[] attributes);

    /// splitCompositeKey splits the specified key into attributes on which the
    /// composite key was formed. Composite keys found during range queries
    /// or partial composite key queries can therefore be split into their
    /// composite parts.
    void /*string, []string, error*/ splitCompositeKey(string compositeKey); // TODO

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
    void /*StateQueryIteratorInterface*/ getQueryResult(string query);

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
    void /*HistoryQueryIteratorInterface*/ getHistoryForKey(string key);

    /// getCreator returns `SignatureHeader.Creator` (e.g. an identity)
    /// of the `SignedProposal`. This is the identity of the agent (or user)
    /// submitting the transaction.
    byte[] getCreator();

    /// getTransient returns the `ChaincodeProposalPayload.Transient` field.
    /// It is a map that contains data (e.g. cryptographic material)
    /// that might be used to implement some form of application-level
    /// confidentiality. The contents of this field, as prescribed by
    /// `ChaincodeProposalPayload`, are supposed to always
    /// be omitted from the transaction and excluded from the ledger.
    byte[][string] getTransient();

    /// getBinding returns the transaction binding, which is used to enforce a
    /// link between application data (like those stored in the transient field
    /// above) to the proposal itself. This is useful to avoid possible replay
    /// attacks.
    byte[] getBinding();

    /// getDecorations returns additional data (if applicable) about the proposal
    /// that originated from the peer. This data is set by the decorators of the
    /// peer, which append or mutate the chaincode input passed to the chaincode.
    byte[][string] getDecorations();

    /// getSignedProposal returns the SignedProposal object, which contains all
    /// data elements part of a transaction proposal.
    SignedProposal getSignedProposal();

    /// getTxTimestamp returns the timestamp when the transaction was created. This
    /// is taken from the transaction ChannelHeader, therefore it will indicate the
    /// client's timestamp and will have the same value across all endorsers.
    Timestamp getTxTimestamp();

    /// setEvent allows the chaincode to propose an event on the transaction
    /// proposal. If the transaction is validated and successfully committed,
    /// the event will be delivered to the current event listeners.
    void setEvent(string name, byte[] payload);

    ChaincodeEvent getEvent();
}


mixin template InvokeHelper(T)
{
    override Response invoke(ChaincodeStub stub)
    {
        import std.traits;

        auto args = stub.getStringArgs();
        if (args.length > 1)
        {
            foreach (member; getSymbolsByUDA!(T, SmartContract))
            {
                if (isFunction!member)
                {
                    // TODO Reflect member and generate call
                    //if (member == args[0])
                    //{
                    //    return success();
                    //}
                }
            }
            return success();
        }
        // TODO Return error
        return success();
    }
}

class DefaultChaincode : Chaincode
{
    Response init(ChaincodeStub stub)
    {
        return success();
    }

    Response invoke(ChaincodeStub stub)
    {
        return success();
    }
}

void start(Chaincode cc);

