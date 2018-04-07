// License: Apache-2.0

#include "fsm.h"

namespace fabric::shim {

template<typename TEvent, typename TState>
void FSM<TEvent, TState>::raiseEvent(TEvent evt) {
    std::lock_guard<std::mutex> lockState(stateMutex);
/*
- Check if transition is valid
- Call before events
- Call after events
*/
}

}
