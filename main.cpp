
#include "aether/network.hpp"
#include "aether/reports.hpp"
#include "aether/system_info.hpp"
#include "aether/terminal_ui.hpp"

#include <charconv>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>

namespace {

volatile std::sig_atomic_t interrupted = 0;

void handle_interrupt(int) {
    interrupted = 1;
}

int read_choice() {
    std::string input;
    std::getline(std::cin, input);

    if (!std::cin) {
        return 0;
    }

    int choice = -1;
    const auto result = std::from_chars(
        input.data(),
        input.data() + input.size(),
        choice
    );

    if (result.ec != std::errc{} || result.ptr != input.data() + input.size()) {
        return -1;
    }

    return choice;
}

std::filesystem::path report_directory() {
    const char* home = std::getenv("HOME");

    if (home == nullptr || *home == '\0') {
        return std::filesystem::current_path() / "aether-reports";
    }

    return std::filesystem::path(home) / "aether-reports";
}

void pause_screen() {
    std::cout << "\n  " << aether::ui::paint(
        "Press ENTER to return to the command deck...",
        aether::ui::Tone::muted
    ) << std::flush;

    std::string ignored;
    std::getline(std::cin, ignored);
}

void dispatch(int choice) {
    switch (choice) {
        case 1:
            aether::display_system_info(aether::collect_system_info());
            break;

        case 2:
            aether::display_network_interfaces(
                aether::collect_network_interfaces()
            );
            break;

        case 3: {
            const auto system = aether::collect_system_info();
            const auto interfaces = aether::collect_network_interfaces();
            const auto path = aether::export_report(
                system,
                interfaces,
                report_directory()
            );

            aether::ui::notice(
                "REPORT SAVED :: " + path.string(),
                aether::ui::Tone::mint
            );
            break;
        }

        case 4:
            aether::ui::notice(
                "COMMAND DECK REFRESHED",
                aether::ui::Tone::cyan
            );
            break;

        default:
            aether::ui::notice(
                "Unknown command. Select 01, 02, 03, 04, or 00.",
                aether::ui::Tone::amber
            );
            break;
    }
}

}

int main() {
    std::signal(SIGINT, handle_interrupt);

    while (!interrupted) {
        aether::ui::clear_screen();
        aether::ui::render_banner();
        aether::ui::render_menu();

        const int choice = read_choice();

        if (interrupted || choice == 0) {
            break;
        }

        try {
            dispatch(choice);
        } catch (const std::exception& error) {
            std::cerr << "AETHER//CORE ERROR: " << error.what() << '\n';
            aether::ui::notice(
                "Operation failed. Check permissions and filesystem access.",
                aether::ui::Tone::red
            );
        }

        if (!interrupted) {
            pause_screen();
        }
    }

    aether::ui::notice("SESSION CLOSED :: SYSTEM UNCHANGED", aether::ui::Tone::violet);
    return interrupted ? 130 : 0;
}

