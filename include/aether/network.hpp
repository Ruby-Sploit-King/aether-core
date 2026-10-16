
#pragma once

#include <string>
#include <vector>

namespace aether {

struct NetworkInterface {
    std::string name;
    std::string address;
    std::string family;
};

std::vector<NetworkInterface> collect_network_interfaces();
void display_network_interfaces(const std::vector<NetworkInterface>& interfaces);

}

