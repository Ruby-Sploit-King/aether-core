
#include "aether/network.hpp"
#include "aether/terminal_ui.hpp"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <memory>
#include <net/if.h>
#include <sys/socket.h>
#include <string>
#include <vector>

namespace aether {

std::vector<NetworkInterface> collect_network_interfaces() {
    ifaddrs* raw_addresses = nullptr;

    if (::getifaddrs(&raw_addresses) != 0) {
        return {};
    }

    const auto release = [](ifaddrs* addresses) {
        if (addresses != nullptr) {
            ::freeifaddrs(addresses);
        }
    };

    std::unique_ptr<ifaddrs, decltype(release)> addresses(raw_addresses, release);
    std::vector<NetworkInterface> results;

    for (auto* current = addresses.get(); current != nullptr; current = current->ifa_next) {
        if (current->ifa_addr == nullptr) {
            continue;
        }

        const int family = current->ifa_addr->sa_family;

        if (family != AF_INET && family != AF_INET6) {
            continue;
        }

        char buffer[INET6_ADDRSTRLEN]{};
        const void* source = nullptr;

        if (family == AF_INET) {
            const auto* address = reinterpret_cast<sockaddr_in*>(current->ifa_addr);
            source = &address->sin_addr;
        } else {
            const auto* address = reinterpret_cast<sockaddr_in6*>(current->ifa_addr);
            source = &address->sin6_addr;
        }

        if (::inet_ntop(family, source, buffer, sizeof(buffer)) == nullptr) {
            continue;
        }

        results.push_back({
            current->ifa_name,
            buffer,
            family == AF_INET ? "IPv4" : "IPv6"
        });
    }

    return results;
}

void display_network_interfaces(const std::vector<NetworkInterface>& interfaces) {
    std::vector<std::string> lines;

    if (interfaces.empty()) {
        lines.emplace_back("No network interface addresses could be read.");
    }

    for (const auto& interface : interfaces) {
        lines.push_back(
            interface.name + "  ::  " + interface.family + "  ::  " + interface.address
        );
    }

    ui::panel("NETWORK//TOPOLOGY", lines);
    ui::status(
        "INTERFACES",
        std::to_string(interfaces.size()) + " address records",
        interfaces.empty() ? ui::Tone::amber : ui::Tone::mint
    );
}

}

