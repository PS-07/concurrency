#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

/*
atmoic provides a way to perform operations on shared data without using mutexes
in contrast to normal variables, atomic variables can be modified by multiple threads concurrently, but without causing inconsistent or incorrect results. 
atomic operations ensure that each read-modify-write operation on a variable is performed atomically, meaning it happens as a single, uninterruptible action.
atomic<T> can be used for built-in types (like int, float, etc.) and user-defined types that are trivially copyable
operations such as ++, -- are atomic in nature and guranteed to be thread-safe
load() and store() methods are used to read and write atomic variables
*/
atomic<int> counter(0);

void increment() {
    for (int i = 0; i < 10000; i++) {
        counter++;
    }
}

int main() {
    thread t1(increment);
    thread t2(increment);

    t1.join();
    t2.join();
    cout<<"Final counter value: "<<counter.load()<<endl;

    return 0;
}