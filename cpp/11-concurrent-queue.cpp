#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

class NormalQueue {
private:
    queue<int> q;

public:
    void enqueue(int value) {
        q.push(value);
    }

    int dequeue() {
        if (q.empty()) {
            cout<<"Queue is empty, cannot dequeue."<<endl;
            return -1;
        }
        int value = q.front();
        q.pop();
        return value;
    }

    int size() {
        return q.size();
    }
};

class ConcurrentQueue {
private:
    queue<int> q;
    mutex mtx;

public:
    void enqueue(int value) {
        lock_guard<mutex> lock(mtx);
        q.push(value);
    }

    int dequeue() {
        lock_guard<mutex> lock(mtx);
        if (q.empty()) {
            cout<<"Queue is empty, cannot dequeue."<<endl;
            return -1;
        }
        int value = q.front();
        q.pop();
        return value;
    }

    int size() {
        lock_guard<mutex> lock(mtx);
        return q.size();
    }
};

int main() {
    NormalQueue nq;

    nq.enqueue(1);
    nq.enqueue(2);
    nq.enqueue(3);

    cout<<"Dequeued: "<<nq.dequeue()<<endl;
    cout<<"Dequeued: "<<nq.dequeue()<<endl;
    cout<<"Dequeued: "<<nq.dequeue()<<endl;
    cout<<"Dequeued: "<<nq.dequeue()<<endl;

    vector<thread> threads;
    for (int i = 0; i < 10000; i++) {
        threads.emplace_back([&nq]() {
            nq.enqueue(rand() % INT32_MAX);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Size is not 10000 because the NormalQueue is not thread-safe
    cout<<"Final queue size: "<<nq.size()<<endl;


    ConcurrentQueue cq;

    vector<thread> thread1;
    for (int i = 0; i < 10000; i++) {
        thread1.emplace_back([&cq]() {
            cq.enqueue(rand() % INT32_MAX);
        });
    }

    for (auto& t : thread1) {
        t.join();
    }

    // Size is 10000 because the ConcurrentQueue is thread-safe
    cout<<"Final queue size: "<<cq.size()<<endl;

    return 0;
}
