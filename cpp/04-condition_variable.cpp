#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace std::chrono;

/*
condition_variable is used to block a thread until a particular condition is met
it is used to synchronize the execution of multiple threads
it notifies other thread(s) waiting on some condition to wake up
a mutex is needed to use condition_variable
if a thread wants to wait for a condition to be met:
    - it must first lock the mutex
    - then call cv.wait(mutex, condition) 
      if condition == false, the thread will be blocked until the condition is met
      if condition == true, the thread will continue execution (skips waiting)
    - when the cv is notified, the thread will wake up and check the condition again
      if condition == true, the thread will continue execution
      if condition == false, the thread will be blocked again

sleep vs wait
sleep is a thread function, it is used to block the current thread for a specified amount of time, it keeps the lock while sleeping
wait is a condition_variable function, it is used to block the current thread until a condition is met, it releases the lock while waiting
*/
condition_variable cv;
mutex mtx;
int balance = 0;

/*
    t1 - execute -> notify_one() / notify_all()
    t2 - wait
    t3 - wait
*/

void deposit(int amount) {
    lock_guard<mutex> lock(mtx);
    balance += amount;
    cout<<"Deposited "<<amount<<", new balance: "<<balance<<endl;
    cv.notify_all(); // or notify_one() to wake up one waiting thread
}

void withdraw(int amount) {
    unique_lock<mutex> lock(mtx);
    cv.wait(lock, [] { return balance != 0; });

    if (balance >= amount) {
        balance -= amount;
        cout<<"Withdrew "<<amount<<", new balance: "<<balance<<endl;
    } else {
        cout<<"Insufficient funds to withdraw "<<amount<<endl;
    }
}

int main() {
    thread t1(withdraw, 110);
    thread t2(withdraw, 200);
    thread t3(deposit, 300);

    t1.join();
    t2.join();
    t3.join();
    cout<<"Final balance: "<<balance<<endl;
    this_thread::sleep_for(seconds(3));

    
    balance = 0;
    thread t4(deposit, 200);
    this_thread::sleep_for(seconds(1));
    thread t5(withdraw, 200);

    t4.join();
    t5.join();
    cout<<"Final balance: "<<balance<<endl;
    this_thread::sleep_for(seconds(3));
    

    balance = 0;
    thread t6(withdraw, 100);
    this_thread::sleep_for(seconds(1));
    thread t7(deposit, 100);

    t6.join();
    t7.join(); 
    cout<<"Final balance: "<<balance<<endl;

    return 0;
}