#include <boost/test/unit_test.hpp>

#include <thread>
#include "channel.h"

BOOST_AUTO_TEST_SUITE(channel_test)

BOOST_AUTO_TEST_CASE(SingleThreadTest)
{
    fabric::shim::Channel<int> channel;
    BOOST_CHECK_MESSAGE(channel.empty(), "channel not empty");
    BOOST_CHECK_MESSAGE(!channel.closed(), "channel is closed");
    channel.put(5);
    BOOST_CHECK_MESSAGE(!channel.empty(), "channel empty");
    channel.put(6);
    channel.put(7);
    BOOST_CHECK_MESSAGE(!channel.empty(), "channel empty");
    BOOST_CHECK(channel.get() == 5);
    BOOST_CHECK(channel.get() == 6);
    BOOST_CHECK(channel.get() == 7);
    BOOST_CHECK_MESSAGE(channel.empty(), "channel not empty");
    BOOST_CHECK_MESSAGE(!channel.closed(), "channel is closed");
    channel.close();
    BOOST_CHECK_MESSAGE(channel.closed(), "channel is not closed");
}

BOOST_AUTO_TEST_CASE(MultiThreadTest1)
{
    fabric::shim::Channel<int> channel;
    BOOST_CHECK_MESSAGE(channel.empty(), "channel not empty");
    BOOST_CHECK_MESSAGE(!channel.closed(), "channel is closed");
    std::thread threadObj([&]{
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        channel.put(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        channel.put(2);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        channel.put(3);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    });
    BOOST_CHECK(channel.get() == 1);
    BOOST_CHECK(channel.get() == 2);
    BOOST_CHECK(channel.get() == 3);
    threadObj.join();
    BOOST_CHECK_MESSAGE(channel.empty(), "channel not empty");
}

BOOST_AUTO_TEST_SUITE_END()
