// License: Apache-2.0

#if !defined(FABRIC_SHIM_FSM_H)
#define FABRIC_SHIM_FSM_H

#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace fabric::shim {

// Assumes:
// TEvent::Type is type of event
// TEvent::type() returns value
template<typename TEvent, typename TState>
class FSM
{
public:
    // TEvent::Type is the type of the event.
    typedef typename TEvent::Type EventType;

    // EventData is passed to callbacks
    struct EventData {
        FSM<TEvent, TState> *fsm;
        EventType evt;
        TState src;
        TState dst;
        bool canceled;
        bool async;
    };

    enum EventKind { Before, Leave, Enter, After };

    struct EventDesc {
        EventType evt;
        TState src;
        TState dst;

        EventDesc(EventType evt, TState src, TState dst) : evt(evt), src(src), dst(dst) {
        }
    };

    struct Callback {
        EventKind kind;
        std::optional<EventType> evt;
        std::function<void(EventData&)> cb;

        Callback(EventKind kind, std::optional<EventType> evt, std::function<void(EventData&)> cb) : kind(kind), evt(evt), cb(cb) {
        }
    };

private:
    struct EventKey {
        EventType evt;
        TState state;

        EventKey(EventType evt, TState state) : evt(evt), state(state) { }

       int operator<(const EventKey key) const {
           return (evt < key.evt) || (evt == key.evt && state < key.state);
       }
    };

    TState state;
    const EventDesc* evtDesc;
    const size_t evtDescCnt;
    const Callback* callback;
    const size_t callbackCnt;
    std::map<EventKey, TState> transitions;

    // Guards access to the current state.
    std::mutex stateMutex;

    // Guards access to ...
    std::mutex eventMutex;

public:
    FSM(TState initial, const EventDesc* evtDesc, const size_t evtDescCnt, const Callback* cb, const size_t cbCnt)
        : state(initial), evtDesc(evtDesc), evtDescCnt(evtDescCnt), callback(cb), callbackCnt(cbCnt) {
        // Initialize transitions
        for (size_t i = 0; i < evtDescCnt; i++) {
            EventKey key(evtDesc[i].evt, evtDesc[i].src);
            transitions[key] = evtDesc[i].dst;
        }
    }

    /// Returns the current state of the FSM.
    TState current() {
        return this.state;
    }

    /// Returns true if state is the current state.
    bool isCurrentState(TState state) {
        return this.state == state;
    }

    void raiseEvent(TEvent evt);
};

}

#endif // !defined(FABRIC_SHIM_FSM_H)

