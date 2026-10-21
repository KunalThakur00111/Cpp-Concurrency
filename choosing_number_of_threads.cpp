#include <iostream>
#include <thread>

int main(){
    int number = std::thread::hardware_concurrency();// gives hint about the number of hardware threads context available.
    //                                               // it might report a number of logical cpu threads. and it can return 0
    //                                                   when it cannot provide a meaningful number. it is not a promise
    //                                                   this many of threads will run simultaneously or that using this many will
    //                                                   be fastest.
    std::cout<<number<<std::endl;
}