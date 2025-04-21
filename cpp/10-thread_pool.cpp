#include <iostream>
#include <queue>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace std;

/*
descibe ThreadPool class
ThreadPool is a class that manages a pool of worker threads to execute tasks concurrently.
It allows you to submit tasks (functions) to be executed by the threads in the pool.
The class uses a queue to store the tasks and a condition variable to notify the worker threads when there are tasks available.
The ThreadPool constructor creates a specified number of worker threads that run in a loop, waiting for tasks to be submitted.
When a task is submitted, it is added to the queue, and one of the worker threads is notified to execute it.
The worker threads run in a loop, waiting for tasks to be available in the queue. When a task is available, a worker thread takes it from the queue and executes it.
The destructor of the ThreadPool class sets a flag to indicate that the pool is done, notifies all worker threads, and waits for them to finish executing their tasks before exiting.
*/
class ThreadPool {
private:
    vector<thread> workers;
    queue<function<void()>> task_queue;
    mutex queue_mutex;
    condition_variable cv;
    atomic<bool> done;

    void worker() {
        while (true) {
            function<void()> task;
            {
                unique_lock<mutex> lock(queue_mutex);
                cv.wait(lock, [this]() { return !task_queue.empty() || done; });
                // if done is true and task_queue is empty, exit the loop
                if (done && task_queue.empty()) {
                    return;
                }
                task = task_queue.front();
                task_queue.pop();
            }
            task();
        }
    }

public:
    ThreadPool(int num_threads) : done(false) {
        for (int i = 0; i < num_threads; i++) {
            // &ThreadPool::worker - pointer to member function
            // this - we need to provide an object (ThreadPool instance) on which to call the member function worker()
            // workers.push_back(thread(&ThreadPool::worker, this));
            workers.emplace_back(&ThreadPool::worker, this);
        }
    }

    // stop all worker threads gracefully by finishing the tasks in the queue for a clean exit
    ~ThreadPool() {
        done = true;                    // stop all threads
        cv.notify_all();                // wake up all waiting threads 

        for (auto &worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    void submit_task(function<void()> task) {
        {
            lock_guard<mutex> lock(queue_mutex);
            task_queue.push(task);
        }
        cv.notify_one();
    }
};

mutex cout_mutex;

void example_task(int id) {
    {
        lock_guard<mutex> lock(cout_mutex);
        cout<<"Task "<<id<<" is being executed by thread "<<this_thread::get_id()<<endl;
    }
    this_thread::sleep_for(chrono::seconds(1));
    {
        lock_guard<mutex> lock(cout_mutex);
        cout<<"Task "<<id<<" completed."<<endl;
    }
}

int main() {
    ThreadPool pool(4);

    for (int i = 1; i <= 10; i++) {
        pool.submit_task([i]() { example_task(i); });
    }

    return 0;
}
