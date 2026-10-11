#include <iostream>
#include <thread>
class abc{
    public:
    int num;
    void operator()(){
        std::cout<<"we make object callable\n";
    }
};
void func(){
    std::cout<<"some ordinary function\n";
}
int main(){
    abc f;
    int n=10;
    //f() --> we can do this becuase of 'operator()', second bracket takes arguments

    std::thread my_thread(f);// here also copy is passing of object f

    // std:: thread my_thread((abc())); // we can do this too, it makes temp object
    // std:: thread my_thread{abc()}; // same here 

    std::thread my_thread2(func);

    std::thread my_thread3([n]{
        std::cout<<"lamda function we can also pass as task"<<n<<std::endl;
        // copy of n is pass inside this lamda function, if we did &n then original one paas
    });

    my_thread.join();
    my_thread2.join();
    my_thread3.join();

    return 0;
}