#include <iostream>
#include <thread>
#include <utility>

void f1(std::string s){

}
void f2(std::string& s){
    
}
void f3(const std::string s){
    
}
int main(){
    char buffer[1024] = "42";
    std::thread t1(f1,buffer);
    std::string original = "hello";
    std::thread t2(f1,original);// valid

    //std::thread t3(f2,original);   // invalid because

    /*
    the above is failed to compile becuase what is happening is it store the original string
    like, string copy_string=original;

    now without std::ref , it will now again copy when function invoking, so it is copying two times
    to avoid that happens is f2(std::move(stored_copy)); and std::move(stored_copy) which is rvalue
    and non const ref value to the r value is invalid.

    it is design choice, thats why it is happening
    */
    t1.detach();

}