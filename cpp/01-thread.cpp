#include <iostream>
#include <thread>

using namespace std;

int num = 0;

void inc(int range) {
    for (int i = 0; i < range; i++) {
        num++;
    }
}

int main() {
    thread t1(inc, 10000);      // create a thread and starts execution
    t1.join();                  // main thread waits for t1 to finish
    inc(10000);                 // starts execution in main thread
    cout<<"num : "<<num<<endl;

    /*
        main      t2
    1   load 0    
    2   inc 1     load 0
    3   store 1   inc 1
    4             store 1
    */
    num = 0;
    thread t2(inc, 10000);
    inc(10000);                 // data race condition since both main and t2 are executing inc() parallelly       
    t2.join();
    cout<<"num: "<<num<<endl;

    num = 0;
    inc(10000);
    cout<<"num: "<<num<<endl;
    thread t3(inc, 10000);
    t3.join();
    cout<<"num: "<<num<<endl;

    num = 0;
    thread t4(inc, 10000);
    t4.join();
    // t4.join();               // error: double join terminates program
    if (t4.joinable()) {        // check if thread is joinable
        t4.join();
    }
    cout<<"num: "<<num<<endl;

    // not using detach is not recommended as it will terminate the program
    num = 0;
    thread t5(inc, 10000);
    t5.detach();                // detaches the thread from parent thread i.e. parent thread would not wait for t5 to finish
    // t5.detach();             // error: double detach terminates program
    if (t5.joinable()) {        // check if thread is joinable
        t5.detach();
    }
    cout<<"num: "<<num<<endl;

    num = 0;
    inc(10000);
    thread t6(inc, 10000);
    t6.detach();
    cout<<"num: "<<num<<endl;

    return 0;
}
