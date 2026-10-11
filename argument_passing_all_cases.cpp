#include <iostream>
#include <thread>
#include <string>
#include <functional>
#include <utility>

void read_only(const std::string& s){
    std::cout<<"Function reads: "<<s<<"\n";
}

void change_by_value(std::string s){
    std::cout<<"Before change: "<<s<<"\n";
    s="changed inside function";
    std::cout<<"After change: "<<s<<"\n";
}

void change_by_reference(std::string& s){
    s="changed inside function";
    std::cout<<"Referenced string: "<<s<<"\n";
}

void print_arguments(int number,const std::string& s){
    std::cout<<"Number: "<<number<<", string: "<<s<<"\n";
}

class Worker{
public:
    std::string message="before work";

    void set_message(const std::string& s){
        message=s;
    }
};

class Task{
public:
    void operator()(int number,const std::string& s) const{
        print_arguments(number,s);
    }
};

// Unsafe: buffer may be destroyed before the worker reads it.
// void unsafe_buffer(){
//     char buffer[1024]="Hello";
//     std::thread t(read_only,buffer);
//     t.detach();
// }

int main(){

    std::cout<<"\n--- 0. Normal call with a copied string ---\n";
    {
        std::string original="Hello";
        std::string copied=original;

        change_by_reference(copied); // Valid: copied is an lvalue.

        std::cout<<"Original: "<<original<<"\n"; // Hello
        std::cout<<"Copied: "<<copied<<"\n";     // Changed

        // Error: non-const & cannot accept an rvalue.
        // change_by_reference(std::move(copied));
    }

    std::cout<<"\n--- 1. Passing multiple arguments ---\n";
    {
        std::thread t(print_arguments,3,"Hello");
        // "Hello" stays alive; the worker can convert it to a string.
        t.join();
    }

    std::cout<<"\n--- 2. Const reference, but thread stores a copy ---\n";
    {
        std::string original="Hello";
        std::thread t(read_only,original); // Stores its own string copy.

        original="changed by main"; // Does not change the stored copy.
        t.join();

        std::cout<<"Original: "<<original<<"\n";
        // Function reads Hello; original is changed by main.
    }

    std::cout<<"\n--- 3. By value: original does not change ---\n";
    {
        std::string original="Hello";
        std::thread t(change_by_value,original);
        t.join();

        std::cout<<"Original: "<<original<<"\n"; // Hello
    }

    std::cout<<"\n--- 4. Reference + std::ref: original changes ---\n";
    {
        std::string original="Hello";

        // Error: thread offers its stored copy as an rvalue.
        // std::thread t(change_by_reference,original);

        std::thread t(change_by_reference,std::ref(original));
        t.join();

        std::cout<<"Original: "<<original<<"\n"; // Changed
    }

    std::cout<<"\n--- 5. By value + std::ref: original does not change ---\n";
    {
        std::string original="Hello";
        std::thread t(change_by_value,std::ref(original));
        // The function's by-value parameter still makes its own copy.
        t.join();

        std::cout<<"Original: "<<original<<"\n"; // Hello
    }

    std::cout<<"\n--- 6. Const reference + std::ref or std::cref ---\n";
    {
        std::string original="Hello";

        std::thread t1(read_only,std::ref(original));
        t1.join();

        std::thread t2(read_only,std::cref(original));
        t2.join();
        // Both read original. Main does not modify it while they run.
    }

    std::cout<<"\n--- 7. Buffer: store its address, then join ---\n";
    {
        char buffer[1024]="Hello from buffer";
        std::thread t(read_only,buffer); // Stores a pointer, not the array.
        t.join(); // Buffer stays alive until the worker finishes.
    }

    std::cout<<"\n--- 8. Buffer: make a string before starting ---\n";
    {
        char buffer[1024]="Hello";
        std::thread t(read_only,std::string(buffer));
        // Reads buffer now; worker gets independent string data.

        buffer[0]='Y';
        t.join();

        std::cout<<"Buffer: "<<buffer<<"\n"; // Yello
        // Function reads Hello.
    }

    std::cout<<"\n--- 9. std::move + by value ---\n";
    {
        std::string original="Hello";
        std::thread t(change_by_value,std::move(original));
        t.join();

        // Original exists, but do not assume its contents after moving.
        original="reused after moving";
        std::cout<<"Original: "<<original<<"\n";

        // Error: non-const & cannot accept the offered rvalue.
        // std::thread wrong(change_by_reference,std::move(original));
    }

    std::cout<<"\n--- 10. std::move + const reference ---\n";
    {
        std::string original="Hello";
        std::thread t(read_only,std::move(original));
        // Moves into storage; function refers to the stored string.
        t.join();
    }

    std::cout<<"\n--- 11. Member function with an argument ---\n";
    {
        Worker w;
        std::thread t(&Worker::set_message,&w,"changed by member function");
        t.join();

        std::cout<<"Object's message: "<<w.message<<"\n";
    }

    std::cout<<"\n--- 12. Function object with arguments ---\n";
    {
        std::thread t(Task{},7,"function object argument");
        t.join();
    }

    std::cout<<"\n--- 13. Lambda with arguments ---\n";
    {
        std::thread t([](int number,const std::string& s){
            print_arguments(number,s);
        },9,"lambda argument");

        t.join();
    }

    std::cout<<"\nAll cases finished\n";
    return 0;
}

