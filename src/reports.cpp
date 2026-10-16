
#include "aether/reports.hpp"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

namespace aether {
namespace {

std::string escape_json(const std::string& value) {
    std::ostringstream output;

    for (const unsigned char character : value) {
        switch (character) {
            case '"': output << "\\\""; break;
            case '\\': output << "\\\\"; break;
            case '\b': output << "\\b"; break;
            case '\f': output << "\\f"; break;
            case '\n': output << "\\n"; break;
            case '\r': output << "\\r"; break;
            case '\t': output << "\\t"; break;
            default:
                if (character < 0x20) {
                    output << "\\u"
                           << std::hex << std::setw(4) << std::setfill('0')
                           << static_cast<int>(character)
                           << std::dec;
                } else {
                    output << static_cast<char>(character);
                }
        }
    }

    return output.str();
}

void write_field(std::ofstream& file, const std::string& key, const std::string& value) {
    file << "    \"" << escape_json(key) << "\": \""
         << escape_json(value) << "\"";
}

}

std::filesystem::path export_report(
    const SystemSnapshot& system,
    const std::vector<NetworkInterface>& interfaces,
    const std::filesystem::path& directory
) {
    std::filesystem::create_directories(directory);

    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();

    auto path = directory / ("aether-report-" + std::to_string(timestamp) + ".json");

    for (int suffix = 1; std::filesystem::exists(path); ++suffix) {
        path = directory / (
            "aether-report-" + std::to_string(timestamp) + "-" +
            std::to_string(suffix) + ".json"
        );
    }

    const auto temporary = path.string() + ".tmp";
    std::ofstream file(temporary, std::ios::out | std::ios::trunc);

    if (!file) {
        throw std::runtime_error("Could not create temporary report file");
    }

    file << "{\n  \"application\": \"AETHER//CORE\",\n";
    file << "  \"system\": {\n";

    const std::vector<std::pair<std::string, std::string>> fields{
        {"hostname", system.hostname},
        {"os", system.operating_system},
        {"kernel", system.kernel},
        {"architecture", system.architecture},
        {"uptime", system.uptime},
        {"cpu_cores", system.cpu_cores},
        {"memory_total", system.memory_total},
        {"memory_available", system.memory_available},
        {"root_disk_free", system.root_disk_free}
    };

    for (std::size_t index = 0; index < fields.size(); ++index) {
        file << "    ";
        write_field(file, fields[index].first, fields[index].second);
        file << (index + 1 == fields.size() ? "\n" : ",\n");
    }

    file << "  },\n  \"network_interfaces\": [\n";

    for (std::size_t index = 0; index < interfaces.size(); ++index) {
        const auto& interface = interfaces[index];

        file << "    {\n      ";
        write_field(file, "name", interface.name);
        file << ",\n      ";
        write_field(file, "family", interface.family);
        file << ",\n      ";
        write_field(file, "address", interface.address);
        file << "\n    }";
        file << (index + 1 == interfaces.size() ? "\n" : ",\n");
    }

    file << "  ]\n}\n";
    file.close();

    if (!file) {
        std::filesystem::remove(temporary);
        throw std::runtime_error("Failed while writing the report");
    }

    std::filesystem::rename(temporary, path);
    return path;
}

}

