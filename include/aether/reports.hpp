
#pragma once

#include "aether/network.hpp"
#include "aether/system_info.hpp"

#include <filesystem>

namespace aether {

std::filesystem::path export_report(
    const SystemSnapshot& system,
    const std::vector<NetworkInterface>& interfaces,
    const std::filesystem::path& directory
);

}

