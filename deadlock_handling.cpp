// The four lock cases, using only two Boxes and three nodes.
// Compile: g++ -std=c++17 -pthread lock_order_cases.cpp -o lock_order_cases

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <utility>

// CASE 1: We already know BOTH objects.
// Let scoped_lock acquire both mutexes safely.
struct Box {
    int id;
    int value;
    std::mutex mutex;

    Box(int id_, int value_) : id(id_), value(value_) {}
};

void swap_together(Box& a, Box& b) {
    if (&a == &b) return;

    std::scoped_lock lock(a.mutex, b.mutex);
    std::swap(a.value, b.value);
} // Both locks released.

// CASE 2: We acquire the locks ONE BY ONE.
// Every thread must use the SAME order: smaller ID first.
void swap_in_order(Box& a, Box& b) {
    if (&a == &b) return;
    if (a.id == b.id) throw std::logic_error("Boxes need different IDs");

    Box* first  = (a.id < b.id) ? &a : &b;
    Box* second = (a.id < b.id) ? &b : &a;

    std::lock_guard<std::mutex> lock_first(first->mutex);
    std::lock_guard<std::mutex> lock_second(second->mutex);
    std::swap(a.value, b.value);
} // Both locks released.

// In this teaching example, all three nodes stay alive until main ends.
// Each node's mutex protects BOTH value and next.
struct Node {
    int value;
    Node* next;
    std::mutex mutex;

    Node(int value_, Node* next_) : value(value_), next(next_) {}
};

// CASE 3: We do not know the NEXT node yet, but all code moves
// FORWARD through the list. Nobody locks nodes in reverse order.
void update_forward(Node& current) {
    std::lock_guard<std::mutex> lock_current(current.mutex);

    Node* next = current.next; // Now we discover the next node.
    if (next == nullptr) return;

    std::lock_guard<std::mutex> lock_next(next->mutex);
    ++current.value;
    ++next->value;
} // Release next, then current.

// Another operation can change a link, but it must lock the current node.
void change_next(Node& current, Node* new_next) {
    if (new_next == &current) {
        throw std::logic_error("A node cannot point to itself here");
    }

    std::lock_guard<std::mutex> lock(current.mutex);
    current.next = new_next;
}

// CASE 4: The next node may CHANGE before we lock it.
// First discover it, then release the first lock.
Node* discover_next(Node& current) {
    std::lock_guard<std::mutex> lock(current.mutex);
    return current.next;
}// unlocks here

// Now acquire BOTH locks and check whether our discovery is still true.
// Returns false if somebody changed current.next in the meantime.
bool update_if_still_linked(Node& current, Node* next) {
    if (next == nullptr) return false;
    if (next == &current) {
        throw std::logic_error("A node cannot point to itself here");
    }

    std::scoped_lock lock(current.mutex, next->mutex);
    if (current.next != next) return false;

    ++current.value;
    ++next->value;
    return true;
}

void update_unknown_next(Node& current) {
    while (true) {
        Node* next = discover_next(current);
        if (next == nullptr) return;

        if (update_if_still_linked(current, next)) return;

        // The link changed. Try again with the new next node.
    }
}

int main() {
    Box a{1, 10};
    Box b{2, 20};

    // Argument order is reversed, but both calls are safe.
    std::thread t1([&] { swap_together(a, b); });
    std::thread t2([&] { swap_together(b, a); });
    t1.join();
    t2.join();
    std::cout << "Case 1: a=" << a.value << ", b=" << b.value << '\n';

    std::thread t3([&] { swap_in_order(a, b); });
    std::thread t4([&] { swap_in_order(b, a); });
    t3.join();
    t4.join();
    std::cout << "Case 2: a=" << a.value << ", b=" << b.value << '\n';

    // Fixed nodes: first -> middle -> last.
    Node last{30, nullptr};
    Node middle{20, &last};
    Node first{10, &middle};

    std::thread t5([&] { update_forward(first); });
    std::thread t6([&] { update_forward(middle); });
    t5.join();
    t6.join();
    std::cout << "Case 3: first=" << first.value
              << ", middle=" << middle.value
              << ", last=" << last.value << '\n';

    // We saw first -> middle, but the link changes to first -> last.
    Node* old_next = discover_next(first);
    change_next(first, &last);

    // The old choice is rejected. Then we retry using the new link.
    bool accepted_old_link = update_if_still_linked(first, old_next);
    std::cout << "Case 4: accepted old link? "
              << std::boolalpha << accepted_old_link << '\n';

    update_unknown_next(first);
    std::cout << "Case 4: first=" << first.value
              << ", last=" << last.value << '\n';
}

/*
1) lock both mutex together with scoped_lock
2) if can't lock them together, lock them in particular order everytime
3) if we don't even know which mutex will be next then, lock A, then discovers B, unlock A, and now try to lock both together with scoped_lock
*/
