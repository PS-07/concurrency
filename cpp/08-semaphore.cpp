#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <semaphore>

using namespace std;

/*
counting_semaphore is a synchronization primitive that allows a limited number of threads to access a resource at the same time
counting_semaphore<max_count> sem(inital_count);
max_count = max no of threads that can access the resource at the same time
initial_count = inital value of the semaphore

sem.acquire() -> decrements the semaphore count, if the count is greater than 0, the thread can proceed
acquire() {
    while (1) {
        atomic {
            if (count > 0) {
                count--;
                return;
            }
        }
    }
}

sem.release() -> increments the semaphore count, if the count is less than max_count, it wakes up one of the waiting threads
release() {
    atomic {
        if (count < max_count) {
            count++;
            return;
        }
    }
}

binary_semaphore is a special case of counting_semaphore with max_count = 1
binary_semaphore sem(0); // initially locked
binary_semaphore sem(1); // initially unlocked
it behave like a mutex
*/

counting_semaphore<3> sem(2);
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

binary_semaphore sem1(1);  // allows one thread at a time
int counter = 0;

void increment(int id) {
    for (int i = 0; i < 10; i++) {
        sem1.acquire();  // lock
        counter++;
        sem1.release();  // unlock
    }
}


int main() {
    vector<thread> threads;

    for (int i = 1; i <=6; i++) {
        // emplace_back is preferred over push_back as push_back initializes an object and then copies it
        // emplace_back constructs the object in place, avoiding the copy
        // emplace_back is more efficient than push_back
        // threads.push_back(thread(task, i));
        threads.emplace_back(task, i);
    }

    for (auto &t : threads) {
        t.join();
    }

    thread t1(increment, 1);
    thread t2(increment, 2);

    t1.join();
    t2.join();
    cout<<"Final counter: "<<counter<<endl;

    return 0;
}