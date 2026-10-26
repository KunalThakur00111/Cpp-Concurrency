#include <algorithm>
#include <iostream>
#include <list>
#include <mutex>
#include <optional>
#include <stack>
#include <thread>

// hold the lock from the first step that depends on the shared data through the last step that changes it.



// This example uses int so returning a value from try_pop() cannot throw.
class ThreadSafeStack {
public:
    std::stack<int> data_;
    std::mutex m_; // One mutex protects the whole stack.

    public:

    void push(int value) {
        std::lock_guard<std::mutex> lock(m_);
        data_.push(value);
    } // m_ is unlocked here

    std::optional<int> try_pop() {
        std::lock_guard<std::mutex> lock(m_);

        // Checking, reading, and removing are ONE operation.
        if (data_.empty()) {
            return std::nullopt;
        }

        int value = data_.top();
        data_.pop();
        return value;
    } // m_ is unlocked here, including on the early return
};

class ThreadSafeList {
public:
    void push_back(int value) {
        std::lock_guard<std::mutex> lock(m_);
        data_.push_back(value);
    }

    bool erase_first(int value) {
        std::lock_guard<std::mutex> lock(m_);

        // Finding and erasing are ONE operation.
        auto it = std::find(data_.begin(), data_.end(), value);
        if (it == data_.end()) {
            return false;
        }

        data_.erase(it);
        return true;
    }

private:
    std::list<int> data_;
    std::mutex m_; // One mutex protects the whole list.
};

int consume_all_available(ThreadSafeStack& stack) {
    int total = 0;

    while (true) {
        auto value = stack.try_pop();
        if (!value) {
            break;
        }

        // The stack is unlocked while this thread processes its own value.
        total += *value;
    }

    return total;
}

int main() {
    ThreadSafeStack stack;
    for (int i = 1; i <= 10; ++i) {
        stack.push(i);
    }

    int first_total = 0;
    int second_total = 0;

    std::thread first([&] { first_total = consume_all_available(stack); });
    std::thread second([&] { second_total = consume_all_available(stack); });

    first.join();
    second.join();

    // Each item was taken once. The split between threads can vary.
    std::cout << "Stack total: " << first_total + second_total << '\n';

    ThreadSafeList list;
    list.push_back(42);

    bool first_erased = false;
    bool second_erased = false;

    std::thread third([&] { first_erased = list.erase_first(42); });
    std::thread fourth([&] { second_erased = list.erase_first(42); });

    third.join();
    fourth.join();

}
