#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>

using namespace std;

const int max_buffer_size = 10;
mutex mtx;
condition_variable cv;
deque<int> buffer;
bool done = false;

void producer(int val) {
    while (val > 0) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return buffer.size() < max_buffer_size; });
        buffer.push_back(val);
        cout<<"Produced: "<<val<<endl;
        val--;
        lock.unlock();
        cv.notify_one();
    }

    unique_lock<mutex> lock(mtx);
    done = true;
    lock.unlock();
    cv.notify_all();
}

void consumer() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return !buffer.empty() || done; });

        // exit if done and buffer is empty
        if (buffer.empty() && done) {
            break; 
        }

        int val = buffer.front();
        buffer.pop_front();
        cout<<"Consumed: "<<val<<endl;
        lock.unlock();
        cv.notify_one();
    }
}


int main() {
    thread t1(producer, 20);
    thread t2(consumer);

    t1.join();
    t2.join();

    return 0;
}