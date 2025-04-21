#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

mutex mtx1;
mutex mtx2;

void func1() {
    lock_guard<mutex> lock1(mtx1);
    this_thread::sleep_for(chrono::milliseconds(100));
    lock_guard<mutex> lock2(mtx2);
    cout<<"Thread 1 acquired both mutexes"<<endl;
}

/*
this will cause a deadlock because:
Thread 1 locks mtx1 and waits for mtx2
Thread 2 locks mtx2 and waits for mtx1
Thread 1 and Thread 2 are waiting for each other to release the mutexes

void func2() {
    lock_guard<mutex> lock2(mtx2);
    this_thread::sleep_for(chrono::milliseconds(100));
    lock_guard<mutex> lock1(mtx1);
    cout<<"Thread 2 acquired both mutexes"<<endl;
}  
*/

// fix: both threads should lock the mutexes in the same order
void func2() {
    lock_guard<mutex> lock1(mtx1);
    this_thread::sleep_for(chrono::milliseconds(100));
    lock_guard<mutex> lock2(mtx2);
    cout<<"Thread 2 acquired both mutexes"<<endl;
}

int main() {
    thread t1(func1);
    thread t2(func2);

    t1.join();
    t2.join();
    cout<<"Both threads finished execution"<<endl;

    return 0;
}