#include <iostream>
#include <thread>
#include <chrono> 

void hello(){
    std::cout << "t1\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout<<"t1 again probably\n";
}

int main(){
    std::thread t(hello);// starts the new thread that runs hello();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // main() thread can do other works 
    std::cout<<"main\n";

    t.join();// makes current main() threads wait until t thread finish its execution
    return 0;
}