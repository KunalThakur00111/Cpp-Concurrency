#include <iostream>
#include <thread>
#include <chrono>
#include <utility>

void some_function(){

}
void some_other_function(){

}

std::thread f(){
    std::thread t(some_function);
    return t;// we do not need to write std::move() , as t was a local , so move is automatically 
}

void f2(std::thread t){}

void g(){
    f2(std::thread(some_function));
    std::thread t(some_function);
    f2(std::move(t));
}

class scoped_thread{
    std::thread t;
    public:
    explicit scoped_thread(std::thread t_): t(std::move(t_)) {
        if(!t.joinable()){
            throw std::logic_error("No thread");
        }
    }
    ~scoped_thread(){
        t.join();
    }
    scoped_thread(scoped_thread const& )= delete;
    scoped_thread& operator=(scoped_thread const& ) = delete;
};

class joining_thread{
    std::thread t_;
    public:
    // 1. create an empty joining thread
    joining_thread() noexcept = default;
    
    // 2. start a new thread from a function and its arguments
    template<class Callable, class... Args>

    explicit joining_thread(Callable&& func, Args&&... args) 
        : t_(std::forward<Callable>(func),
             std::forward<Args>(args)...) {}

    // 3. take an existing std::thread 
    explicit joining_thread(std::thread other) noexcept 
        : t_(std::move(other)) {};

    // 4. copy is forbidden
    joining_thread(const joining_thread& ) = delete;
    joining_thread& operator=(const joining_thread&) = delete;

    // 5. move construction: take another joining_thread's thread
    joining_thread(joining_thread&& other) noexcept
        : t_(std::move(other.t_)) {};
    
    // move assignment: join our old thread, then take the new one
    joining_thread& operator=(joining_thread&& other) noexcept {
        if(joinable()) t_.join();
        t_=std::move(other.t_);
        return *this;
    }

    // assn from a std::thread
    joining_thread& operator=(std::thread other) noexcept {
        if(joinable()) t_.join();
        t_=std::move(other);
        return *this;
    }

    ~joining_thread() noexcept {
        if(joinable()){
            t_.join();
        }
    }

    void swap(joining_thread& other) noexcept {
        t_.swap(other.t_);
    }

    bool joinable() const noexcept {
        return t_.joinable();
    }

    // ask which thread we manage
    std::thread::id get_id() const noexcept {
        return t_.get_id();
    }

    void join(){
        t_.join();
    }

    void detach(){
        t_.detach();
    }



};

int main(){

    std::thread t1(some_function);
    std::thread t2 =std::move(t1);

    t1= std::thread(some_other_function);// doesn't require to call std::move() becuase move from temporaries is automatic and implicit

    std::thread t3;
    t3=std::move(t2);
    //t1=std::move(t3);// this will std::terminate() the program becuase atp t1 has already associated with the thread execution of some_other_function();
    // this is done becuase to make consistency 

    std::thread worker = f();

}