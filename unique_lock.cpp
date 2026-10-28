#include <iostream>
#include <thread>
#include <mutex>

int main(){
    
    std::mutex m;
    std::unique_lock<std::mutex> lock(m,std::defer_lock);// lock is associated with m but m is unlock here
    // m is still unlocked
    lock.lock();
    // m is locked

    std::unique_lock<std::mutex> new_lock(std::move(lock));// now new_lock owns this mutex

    new_lock.unlock();// it can unlock before it's scope ends, it is flexible

}