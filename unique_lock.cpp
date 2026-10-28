#include <iostream>
#include <thread>
#include <mutex>

std::mutex some_mutex;

void prepare_data(){};
void do_something(){};

std::unique_lock<std::mutex> get_lock(){
    extern std::mutex some_mutex;// extern says mutex named some_mutex exists elsewhere; use that same mutex , it's a decleration
    //                              it doesn't create a new mutex
    std::unique_lock<std::mutex> lock(some_mutex);
    prepare_data();
    return lock;
}

void process_data(){
    std::unique_lock<std::mutex> lock = get_lock();
    do_something();
}

int main(){

    std::unique_lock<std::mutex> lock(some_mutex,std::defer_lock);// lock is associated with m but m is unlock here
    // m is still unlocked
    lock.lock();
    // m is locked

    std::unique_lock<std::mutex> new_lock(std::move(lock));// now new_lock owns this mutex

    new_lock.unlock();// it can unlock before it's scope ends, it is flexible
    
}