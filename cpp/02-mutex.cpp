#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

int amount = 0;
mutex mtx;

void add_money() {
    /* add in assembly: 
        1. mov eax, [amount]    // load amount into eax (register)
        2. inc eax              // increment eax
        3. mov [amount], eax    // store eax back to amount

    note: this is not atomic operation, so if two threads are executing this code at the same time, 
    they will read the same value of amount and increment it to the same value
    it's called data race condition
    hence, we need to use mutex to make it atomic
    */
    mtx.lock();
    amount++;
    mtx.unlock();      
}

int val = 0;

void task(int range) {
    /*
    lock_guard is a lightweight mutex wrapper that locks the mutex when it is created and unlocks it when it goes out of scope
    it is used to avoid deadlock and make the code cleaner
    you cannot explocity unlock the lock_guard or copy it
    */
    lock_guard<mutex> lock(mtx);
    for (int i = 0; i < range; i++) {
        val++;
    }
}

int val1 = 0;

void task1(int range) {
    /*
    unique_lock is a mutex wrapper that allows you to lock and unlock the mutex manually
    it is used when you need to lock and unlock the mutex multiple times in the same scope
    we can explicitly lock adn unlock the unique_lock unlike lock_guard
    */
   unique_lock<mutex> lock(mtx, defer_lock); // use defer_lock strategy to create a unique_lock without locking the mutex immedeately
   lock.lock();
   for (int i = 0; i < range; i++) {
        val1++;
    }
    // lock.unlock();                       // not needed as it will be unlocaked when it goes out of scope (in destructor of unique_lock) 
}

int main() {
    thread t1(add_money);
    thread t2(add_money);
    t1.join();
    t2.join();
    cout<<"amount: "<<amount<<endl;

    thread t3(task, 10000);
    thread t4(task, 10000);
    t3.join();
    t4.join();

    cout<<"val: "<<val<<endl;

    thread t5(task1, 10000);
    thread t6(task1, 10000);
    t5.join();  
    t6.join();
    cout<<"val1: "<<val1<<endl;

    return 0;
}
