#include <iostream>
#include <mutex>
#include <algorithm>
#include <thread>
#include <list>

std::mutex some_mutex;
std::list<int> some_list;

void add_to_list(int new_val){
    {
        std::lock_guard<std::mutex> lock(some_mutex);
        some_list.push_back(new_val);
    }
}

bool list_contains(int value_to_find){
    {
        std::lock_guard<std::mutex> lock(some_mutex);
        return std::find(some_list.begin(),some_list.end(),value_to_find)!=some_list.end();
    }
}


// trying something intresting
class abc{
    std::mutex mutex_;
    int counter=0;

    public:
    void increment(){
        {
            std::lock_guard<std::mutex> lock(mutex_);
            counter++;
        }
    }
    void decrement(){
        {
            std::lock_guard<std::mutex> lock(mutex_);
            counter--;
        }
    }
    void give_address(int*& output){
        std::lock_guard<std::mutex> lock(some_mutex);
        output=&counter;
    }//unlocked but output still holds the address of counter
};

class some_data{
    int a;
    std::string b;

    public:
    void do_something();
};

class data_wrapper{
    private:
    some_data data;
    std::mutex m;

    public:
    template<typename Function>
    void process_data(Function func){
        std::lock_guard<std::mutex> lock(m);
        func(data);
    }
};

some_data* unprotected;
void malicous_function(some_data& protected_data){
    unprotected=&protected_data;
}

data_wrapper x;

void foo(){
    x.process_data(malicous_function);
    unprotected->do_something();
}

int main(){
    
    return 0;
}