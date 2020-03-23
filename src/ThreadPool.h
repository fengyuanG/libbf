#pragma once

#include <list>
#include <mutex>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <functional>
#include <condition_variable>

using namespace std;

namespace bf {
using Task = function<void(void)>;
class ThreadPool {
public:
    ThreadPool() : _stop(false) {

    }

    ~ThreadPool() {
        _stop = true;
        _condition.notify_all();
        for (auto& th : _threads) {
            th.join();
        }
    }

    bool init(int32_t capacity) {
        for (int i = 0; i < capacity; ++i)
        {
            _threads.push_back(thread([this](){
                while (true) {
                    shared_ptr<Task> task;
                    {
                        unique_lock<mutex> lck(_mutex);
                        _condition.wait(lck, [this] () {
                            return !_queue.empty();
                        });
                        task = _queue.front();
                        _queue.pop_front();
                    }

                    if (nullptr != *task) {
                        (*task)();
                    }
                }
            }));
        }
    }

    bool execute(Task&& task) {
        lock_guard<mutex> lck(_mutex);
        _queue.push_back(make_shared<Task>(task));
        _condition.notify_one();
    }

protected:
private:
    vector<thread>                      _threads;
    list<shared_ptr<Task>>              _queue;
    mutex                               _mutex;
    condition_variable                  _condition;
    atomic<bool>                        _stop;
};

}