#include <iostream>
#include <thread>
#include <future>

using namespace std;

int find_odd(int start, int end) {
    cout<<"Func Thread ID: "<<this_thread::get_id()<<endl;
    int sum = 0;
    for (int i = start; i < end; i++) {
        if (i & 1)
            sum += i;
    }
    return sum;
}

int main() {
    int start = 0;
    int end = 10000;

    // async is used to run a function in the background
    // lanch::async - run the function in a new thread
    // eager execution - the function will be executed immediately
    cout<<"Main Thread ID: "<<this_thread::get_id()<<endl;
    future<int> odd_sum = async(launch::async, find_odd, start, end);
    cout<<"Calculating odd sum in the background..."<<endl;
    int ans = odd_sum.get();         // wait for the result
    cout<<"Odd sum: "<<ans<<endl;   
    cout<<"Done!"<<endl;

    // launch::deferred - run the function in the same thread
    // lazy execution - the function will not be executed until get() is called
    cout<<"Main Thread ID: "<<this_thread::get_id()<<endl;
    future<int> odd_sum2 = async(launch::deferred, find_odd, start, end);
    cout<<"Calculating odd sum in the background..."<<endl;
    int ans2 = odd_sum2.get();         // wait for the result
    cout<<"Odd sum: "<<ans2<<endl;
    cout<<"Done!"<<endl;

    return 0;
}
