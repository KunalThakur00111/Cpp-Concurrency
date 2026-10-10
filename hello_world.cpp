#include <iostream>
#include <thread>
#include <chrono> 

void hello(){
    std::cout << "Hello Concurrent World=";
}

int main(){
    std::thread t(hello);// starts the new thread that runs hello();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // main() thread can do other works 
    std::cout<<"acha\n";

    hello();std::cout<<"oo teri\n";

    t.join();// makes current main() threads wait until t thread finish its execution
    return 0;
}