
#include "aether/network.hpp"
#include "aether/reports.hpp"
#include "aether/system_info.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

int main() {
    const auto snapshot = aether::collect_system_info();

    assert(!snapshot.kernel.empty());
    assert(!snapshot.architecture.empty());
    assert(!snapshot.hostname.empty());

    const auto interfaces = aether::collect_network_interfaces();

    for (const auto& interface : interfaces) {
        assert(!interface.name.empty());
        assert(!interface.address.empty());
        assert(interface.family == "IPv4" || interface.family == "IPv6");
    }

    const auto directory = std::filesystem::temp_directory_path()
        / "aether-core-tests";

    std::filesystem::remove_all(directory);

    const auto first = aether::export_report(snapshot, interfaces, directory);
    const auto second = aether::export_report(snapshot, interfaces, directory);

    assert(std::filesystem::exists(first));
    assert(std::filesystem::exists(second));
    assert(first != second);

    std::ifstream report(first);
    const std::string content{
        std::istreambuf_iterator<char>(report),
        std::istreambuf_iterator<char>()
    };

    assert(content.find("AETHER//CORE") != std::string::npos);
    assert(content.find("network_interfaces") != std::string::npos);

    std::filesystem::remove_all(directory);
    return 0;
}

