#include <boost/test/unit_test.hpp>

#include "fsm.h"

BOOST_AUTO_TEST_SUITE(fsm_test)

enum States {
    Created,
    Init
};

struct Event {
    enum Type {
        Init
    };

    Type type_;
};

BOOST_AUTO_TEST_CASE(PassTest)
{
    fabric::shim::FSM<Event, States> TheFSM(Created, nullptr, 0, nullptr, 0);
    //BOOST_CHECK_EQUAL(4, sqr(2));
}

BOOST_AUTO_TEST_SUITE_END()
