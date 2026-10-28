// Chapter 3.3.1: shared_ptr and thread-safe one-time initialization.
// Build from this folder:
// g++ -std=c++17 -Wall -Wextra -pedantic -pthread lazy_initialization_cases.cpp -o lazy_init

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

// This resource never changes after construction, so concurrent reads are safe.
struct ReadOnlyResource {
    const int value;

    explicit ReadOnlyResource(int v) : value(v) {}
    int read() const { return value; }
};

// CASE 1: shared_ptr basics are shown at the start of main().
// Two shared_ptr objects can own ONE heap object. Neither is a mutex.

// CASE 2: A mutex protects the check and first construction.
// Safe, but EVERY call takes the mutex just to check the pointer.
namespace mutex_version {
    std::shared_ptr<const ReadOnlyResource> resource; // Initially empty.
    std::mutex init_mutex;
    int constructions = 0;

    int use() {
        std::unique_lock<std::mutex> lock(init_mutex);
        if (!resource) {
            resource = std::make_shared<const ReadOnlyResource>(42);
            ++constructions;
        }
        lock.unlock(); // We need no lock to read the completed resource.
        return resource->read();
    }
}

// CASE 3: DO NOT USE this double-checked version.
// The first read of resource has no lock, while another thread may write it.
// It is left as a comment so the runnable program has no data race.
/*
int broken_use() {
    if (!mutex_version::resource) { // Unsafe read of the shared_ptr variable.
        std::lock_guard<std::mutex> lock(mutex_version::init_mutex);
        if (!mutex_version::resource) {
            mutex_version::resource =
                std::make_shared<const ReadOnlyResource>(42);
        }
    }
    return mutex_version::resource->read();
}
*/

// CASE 4: call_once protects creation without our own mutex check each time.
namespace once_version {
    std::shared_ptr<const ReadOnlyResource> resource; // Initially empty.
    std::once_flag init_flag;
    int constructions = 0;

    void initialize() {
        resource = std::make_shared<const ReadOnlyResource>(100);
        constructions++;
    }

    int use() {
        std::call_once(init_flag, initialize);
        return resource->read(); // Safe: initialization is complete and read-only.
    }
}

// CASE 5: Each object has its OWN once_flag and resource.
class LazyService {
    std::once_flag init_flag;
    std::shared_ptr<const ReadOnlyResource> resource;

    void initialize() {
        resource = std::make_shared<const ReadOnlyResource>(200);
    }

public:
    int use() {
        // &LazyService::initialize names a member function.
        // this says which LazyService object should run it.
        std::call_once(init_flag, &LazyService::initialize, this);
        return resource->read();
    }
};

// CASE 6: A function-local static is also initialized once, safely since C++11.
const ReadOnlyResource& get_one_global_resource() {
    static const ReadOnlyResource resource(300);
    return resource;
}

// CASE 7: If the resource CHANGES after initialization, protect those changes.
// call_once would protect construction only; this mutex protects increment().
struct CounterResource {
    int count = 0;
    std::mutex mutex;

    void increment() {
        std::lock_guard<std::mutex> lock(mutex);
        count++;
    }

    int read() {
        std::lock_guard<std::mutex> lock(mutex);
        return count;
    }
};

int main() {
    // CASE 1: Two pointers, one int. Copying the pointer does not copy the int.
    auto first = std::make_shared<int>(5);
    auto second = first;
    *first = 8;
    std::cout << "1. second sees " << *second
              << "; owners = " << first.use_count() << '\n';
    first.reset(); // second still owns the int.
    std::cout << "   after first.reset(), second sees " << *second << '\n';

    // CASE 2: Two threads call the mutex version.
    int mutex_a = 0, mutex_b = 0;
    std::thread a([&] { mutex_a = mutex_version::use(); });
    std::thread b([&] { mutex_b = mutex_version::use(); });
    a.join();
    b.join();
    std::cout << "2. mutex: " << mutex_a << ", " << mutex_b
              << "; constructions = " << mutex_version::constructions << '\n';

    // CASE 3 is commented out above because running it would be unsafe.

    // CASE 4: Two threads call the call_once version.
    int once_a = 0, once_b = 0;
    std::thread c([&] { once_a = once_version::use(); });
    std::thread d([&] { once_b = once_version::use(); });
    c.join();
    d.join();
    std::cout << "4. call_once: " << once_a << ", " << once_b
              << "; constructions = " << once_version::constructions << '\n';

    // CASE 5: First call to this particular object initializes it.
    LazyService service;
    std::cout << "5. class member: " << service.use() << '\n';

    // CASE 6: First call to this function initializes its static object.
    std::cout << "6. local static: " << get_one_global_resource().read() << '\n';

    // CASE 7: Both threads modify the same resource safely.
    CounterResource counter;
    std::thread e([&] { counter.increment(); });
    std::thread f([&] { counter.increment(); });
    e.join();
    f.join();
    std::cout << "7. protected changes: count = " << counter.read() << '\n';
}
