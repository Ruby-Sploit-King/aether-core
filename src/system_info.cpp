
#include "aether/system_info.hpp"
#include "aether/terminal_ui.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/statvfs.h>
#include <sys/utsname.h>
#include <unistd.h>

namespace aether {
namespace {

std::string read_first_line(const std::string& path) {
    std::ifstream file(path);
    std::string line;

    if (file && std::getline(file, line)) {
        return line;
    }

    return "Unavailable";
}

long long memory_value(const std::string& key) {
    std::ifstream file("/proc/meminfo");
    std::string name;
    long long value = 0;
    std::string unit;

    while (file >> name >> value >> unit) {
        if (name == key) {
            return value;
        }
    }

    return -1;
}

std::string format_kib(long long kib) {
    if (kib < 0) {
        return "Unavailable";
    }

    const double gib = static_cast<double>(kib) / 1024.0 / 1024.0;
    std::ostringstream output;
    output << std::fixed << std::setprecision(2) << gib << " GiB";
    return output.str();
}

std::string read_uptime() {
    std::ifstream file("/proc/uptime");
    double seconds = 0.0;

    if (!(file >> seconds)) {
        return "Unavailable";
    }

    const auto total = static_cast<long long>(seconds);
    const auto days = total / 86400;
    const auto hours = (total % 86400) / 3600;
    const auto minutes = (total % 3600) / 60;

    std::ostringstream output;
    output << days << "d " << hours << "h " << minutes << "m";
    return output.str();
}

std::string root_free_space() {
    struct statvfs info {};

    if (::statvfs("/", &info) != 0) {
        return "Unavailable";
    }

    const auto bytes = static_cast<unsigned long long>(info.f_bavail) * info.f_frsize;
    const double gib = static_cast<double>(bytes) / 1024.0 / 1024.0 / 1024.0;

    std::ostringstream output;
    output << std::fixed << std::setprecision(2) << gib << " GiB";
    return output.str();
}

}

SystemSnapshot collect_system_info() {
    utsname details{};

    if (::uname(&details) != 0) {
        throw std::runtime_error("Unable to read kernel information");
    }

    char hostname[256]{};
    const bool has_hostname = ::gethostname(hostname, sizeof(hostname) - 1) == 0;

    const long cores = ::sysconf(_SC_NPROCESSORS_ONLN);
    const long long total = memory_value("MemTotal:");
    const long long available = memory_value("MemAvailable:");

    return {
        has_hostname ? hostname : "Unavailable",
        read_first_line("/etc/os-release"),
        details.release,
        details.machine,
        read_uptime(),
        cores > 0 ? std::to_string(cores) : "Unavailable",
        format_kib(total),
        format_kib(available),
        root_free_space()
    };
}

void display_system_info(const SystemSnapshot& snapshot) {
    ui::panel("SYSTEM//OVERVIEW", {
        "HOSTNAME       :: " + snapshot.hostname,
        "OS RELEASE     :: " + snapshot.operating_system,
        "KERNEL         :: " + snapshot.kernel,
        "ARCHITECTURE   :: " + snapshot.architecture,
        "CPU CORES      :: " + snapshot.cpu_cores,
        "MEMORY TOTAL   :: " + snapshot.memory_total,
        "MEMORY FREE    :: " + snapshot.memory_available,
        "ROOT FREE      :: " + snapshot.root_disk_free,
        "UPTIME         :: " + snapshot.uptime
    });

    ui::status("KERNEL", snapshot.kernel, ui::Tone::cyan);
    ui::status("SESSION", "LOCAL / READ-ONLY", ui::Tone::mint);
}

}

