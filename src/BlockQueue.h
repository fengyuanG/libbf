#pragma once

#include <list>
#include <mutex>
#include <memory>
#include <condition_variable>

using namespace std;

namespace bf {

template<typename T>
class BlockQueue {
public:
    void add(T&& item) {
        lock_guard<mutex> lck(_mutex);
        _queue.push_back(make_shared<T>(item));
        _condition.notify_one();
    }

    T&& get() {
        unique_lock<mutex> lck(_mutex);
        _condition.wait(lck, [this] () {
            return !_queue.empty();
        });
        shared_ptr<T> item = _queue.front();
        _queue.pop_front();
        return std::move(*item);
    }
protected:
private:
    list<shared_ptr<T>>                 _queue;
    mutex                               _mutex;
    condition_variable                  _condition;
};

}