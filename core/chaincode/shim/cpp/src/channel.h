// License: Apache-2.0

#if !defined(FABRIC_SHIM_CHANNEL_H)
#define FABRIC_SHIM_CHANNEL_H

#include <condition_variable>
#include <list>
#include <mutex>

namespace fabric::shim {

// Based on this blog https://st.xorian.net/blog/2012/08/go-style-channel-in-c/
// by Ken Schalk
template<typename Msg>
class Channel
{
private:
    std::list<Msg> queue;
    std::mutex m;
    std::condition_variable cv;
    bool is_closed;

public:
    Channel() : is_closed(false) { }

    void close() {
        std::unique_lock<std::mutex> lock(m);
        is_closed = true;
        cv.notify_all();
    }

    bool closed() {
        std::unique_lock<std::mutex> lock(m);
        return is_closed;
    }

    void put(const Msg &msg) {
        std::unique_lock<std::mutex> lock(m);
        if (is_closed)
            throw std::logic_error("put to closed channel");
        queue.push_back(msg);
        cv.notify_one();
    }

    Msg get() {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [&](){
            return is_closed || !queue.empty();
        });
        if (is_closed)
            ;
        auto result = queue.front();
        queue.pop_front();
        return result;
    }

    bool empty() {
        std::unique_lock<std::mutex> lock(m);
        return queue.empty();
    }
};

}

#endif // !defined(FABRIC_SHIM_CHANNEL_H)

