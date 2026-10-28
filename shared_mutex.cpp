#include <iostream>
#include <thread>
#include <mutex>
#include <utility>
#include <shared_mutex>
#include <map>

class dns_entry{
    public:
    std::string ip_address;
};

class dns_cache{
    std::map<std::string,dns_entry> entries;
    mutable std::shared_mutex entry_mutex;// mutable allows a const function to lock that mutex

    public:
    dns_entry find_entry(const std::string& domain) const {
        std::shared_lock<std::shared_mutex> lock(entry_mutex);// shared lock
        auto it = entries.find(domain);
        
        if(it == entries.end()){
            return dns_entry{};
        }

        return it->second;
    }

    void update_or_add_entry(const std::string& domain, const dns_entry& dns_details){
        std::lock_guard<std::shared_mutex> lock(entry_mutex);// exclusive lock
        entries[domain] = dns_details;
    }
};

int main(){

}