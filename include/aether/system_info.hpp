#pragma once

#include <string>

namespace aether {

struct SystemSnapshot {
    std::string hostname;
    std::string operating_system;
    std::string kernel;
    std::string architecture;
    std::string uptime;
    std::string cpu_cores;
    std::string memory_total;
    std::string memory_available;
    std::string root_disk_free;
};

SystemSnapshot collect_system_info();
void display_system_info(const SystemSnapshot& snapshot);

}
