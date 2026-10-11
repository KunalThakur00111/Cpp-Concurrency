#include <iostream>
#include <thread>
struct f{
    int x;
    // some defination
};
void some_func(){
    int local_variable=0;
    f obj(local_variable);
    std::thread my_thread(obj);
    try{
        // do something in current thread;
    }
    catch(...){
        my_thread.join();
        throw;
    }
    my_thread.join();
}
/*
we have to call my_thread.join() on catch and before scopes end, becuase if exception is throws
in that case rest of below code will skip, so we have to carefully join there to, otherwise 
std::terminate() calls and program exits abruptly.
*/