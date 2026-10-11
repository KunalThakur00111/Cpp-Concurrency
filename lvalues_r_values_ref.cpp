#include <iostream>
#include <string>
#include <utility>

/*
LVALUE:
An expression that identifies an object or function with an identity.
For the examples below, it refers to a particular existing object.
Examples: number, text, values[0], *pointer.

RVALUE:
An expression that either produces a value, such as a temporary result,
or identifies an object whose contents are eligible to be moved from.
Examples: 10, number+5, std::string("Hello"), std::move(text).

Remember:
- These categories describe EXPRESSIONS, not data types.
- Lvalue does not mean "left side"; rvalue does not mean "right side".
- A const object can be an lvalue even though you cannot change it.
- std::move offers an object for moving; it does not itself move anything.
- "Hello" is an lvalue character array, not a std::string object.
*/

void change(int& value){
    value=100;
}

void read(const int& value){
    std::cout<<"Read: "<<value<<"\n";
}

void change_copy(int value){
    value=200;
    std::cout<<"Function's own value: "<<value<<"\n";
}

int make_number(){
    return 50;
}

int main(){

    // 1. Named variables are lvalue expressions.
    int number=10;
    std::string text="Hello";

    number=20;
    text="Hi";

    std::cout<<"Number: "<<number<<"\n";
    std::cout<<"Text: "<<text<<"\n";

    // 2. A literal integer and an arithmetic result are rvalues.
    int a=10;             // 10 is an rvalue.
    int b=number+5;       // number is an lvalue; number+5 is an rvalue.
    int c=make_number();  // This call returns an int value: an rvalue.

    std::cout<<"a, b, c: "<<a<<", "<<b<<", "<<c<<"\n";

    // Invalid for these ordinary integer expressions:
    // 10=30;
    // (number+5)=30;

    // 3. An lvalue can appear on the RIGHT side.
    int copied=number; // number is still an lvalue.
    copied=99;
    std::cout<<"Number: "<<number<<", copied: "<<copied<<"\n";

    // 4. Const does not change the value category.
    const int limit=100; // The expression limit is an lvalue.
    std::cout<<"Limit: "<<limit<<"\n";
    // limit=200; // Error because it is const, not because it is an rvalue.

    // 5. Array elements and dereferenced pointers are also lvalues.
    int values[2]={1,2};
    int* pointer=&number;

    values[0]=5;
    *pointer=30; // Changes number.

    std::cout<<"Array element: "<<values[0]<<"\n";
    std::cout<<"Number through pointer: "<<number<<"\n";

    // 6. Non-const & accepts a suitable lvalue, not an rvalue.
    int& reference=number;
    reference=40; // Changes number.
    change(number); // Changes number to 100.

    std::cout<<"After reference changes: "<<number<<"\n";

    // int& wrong=10;
    // change(10);
    // change(number+5);
    // change(std::move(number));

    // 7. Const & accepts suitable lvalues AND rvalues.
    read(number);   // Lvalue.
    read(10);       // Rvalue.
    read(number+5); // Rvalue.

    const int& temporary_reference=25; // Valid; keeps this temporary alive.
    std::cout<<"Temporary through const reference: "<<temporary_reference<<"\n";
    // temporary_reference=30; // Cannot modify through a const reference.

    // 8. A by-value parameter accepts either and has its own integer.
    change_copy(number);
    change_copy(10);
    std::cout<<"Original number: "<<number<<"\n"; // Still 100.

    // 9. A temporary std::string is an rvalue.
    std::string first=std::string("Hello");
    // first is an lvalue expression; std::string("Hello") is an rvalue.

    // 10. std::move(first) is an rvalue expression referring to first.
    // std::move(first); // By itself, does not move anything.
    std::cout<<"Before actual move: "<<first<<"\n"; // Hello.

    std::string second=std::move(first); // second's constructor performs the move.
    std::cout<<"Moved value: "<<second<<"\n"; // Hello.

    // first still exists. Do not assume its old contents or that it is empty.
    first="Reused";
    std::cout<<"Reused source: "<<first<<"\n";

    // 11. A string literal is an lvalue array. Do not call every literal an rvalue.
    const char* literal="Hello"; // The array becomes a pointer in this initialization.
    std::cout<<"String literal: "<<literal<<"\n";

    return 0;
}
