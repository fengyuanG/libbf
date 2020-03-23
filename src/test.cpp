#include "BlockQueue.h"
#include "ThreadPool.h"
#include <thread>

using namespace bf;

BlockQueue<int>     lqueue;
ThreadPool          threadpool;
atomic<int>         count(0);

int main() 
{
    threadpool.init(10);

    thread th([](){
        while (true) {
            threadpool.execute([](){
                printf("add: %d\n", count.load());
                lqueue.add(count);
                ++count;
            });
        }
    });

    while (true) {
        printf("get: %d\n", lqueue.get());
    }

    return 0;
}