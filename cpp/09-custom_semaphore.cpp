#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

using namespace std;

class CustomSemaphore {
private:
    int count;
    const int max_count;
    mutex mtx;
    condition_variable cv;
public:
    CustomSemaphore(int max_count, int init_count)
        : max_count(max_count), count(init_count) {}

    void acquire() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [&]() { return count > 0; }); 
        count--;
    }

    void release() {
        unique_lock<mutex> lock(mtx);
        if (count < max_count) {
            count++;
            cv.notify_one();
        }
    }
};

CustomSemaphore sem(3, 2);
mutex cout_mtx;

void task(int id) {
    {
        lock_guard<mutex> lock(cout_mtx);
        cout<<"Thread "<<id<<" is trying to acquire the semaphore."<<endl;
    }
    sem.acquire();

    {
        lock_guard<mutex> lock(cout_mtx);
        cout<<"Thread "<<id<<" has acquired the semaphore."<<endl;
    }
    this_thread::sleep_for(chrono::seconds(1));

    {
        lock_guard<mutex> lock(cout_mtx);
        cout<<"Thread "<<id<<" is releasing the semaphore."<<endl;
    }
    sem.release();
}

int main() {
    vector<thread> threads;

    for (int i = 1; i <= 6; i++) {
        threads.emplace_back(task, i);
    }

    for (auto &t : threads) {
        t.join();
    }

    return 0;
}
