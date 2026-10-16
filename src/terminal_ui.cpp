#include "aether/terminal_ui.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

namespace aether::ui {
namespace {

constexpr const char* reset = "\033[0m";
constexpr const char* cyan = "\033[38;2;103;247;255m";
constexpr const char* violet = "\033[38;2;184;140;255m";
constexpr const char* magenta = "\033[38;2;255;100;211m";
constexpr const char* mint = "\033[38;2;127;255;178m";
constexpr const char* amber = "\033[38;2;255;203;107m";
constexpr const char* red = "\033[38;2;255;105;125m";
constexpr const char* muted = "\033[38;2;115;137;158m";
constexpr const char* white = "\033[38;2;222;235;247m";

bool color_enabled() {
    return ::isatty(STDOUT_FILENO) && std::getenv("NO_COLOR") == nullptr;
}

const char* code(Tone tone) {
    switch (tone) {
        case Tone::cyan:
            return cyan;
        case Tone::violet:
            return violet;
        case Tone::magenta:
            return magenta;
        case Tone::mint:
            return mint;
        case Tone::amber:
            return amber;
        case Tone::red:
            return red;
        case Tone::muted:
            return muted;
        case Tone::white:
            return white;
    }

    return reset;
}

std::size_t terminal_width() {
    winsize size{};

    if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 && size.ws_col >= 60) {
        return size.ws_col > 100 ? 100 : size.ws_col;
    }

    return 78;
}

std::string fit(std::string text, std::size_t width) {
    if (text.size() > width) {
        text.resize(width);
    }

    text.append(width - text.size(), ' ');
    return text;
}

}

std::string paint(const std::string& text, Tone tone) {
    if (!color_enabled()) {
        return text;
    }

    return std::string(code(tone)) + text + reset;
}

std::string gradient_title() {
    return paint("A E T H E R", Tone::cyan) + " " +
           paint("//", Tone::muted) + " " +
           paint("C O R E", Tone::magenta);
}

void clear_screen() {
    if (::isatty(STDOUT_FILENO)) {
        std::cout << "\033[2J\033[H";
    }
}

void render_banner() {
    const std::size_t width = terminal_width();
    const std::string inner = " AETHER//CORE :: LINUX COMMAND DECK ";
    const std::string line(width, '=');

    std::cout << paint(line, Tone::violet) << '\n';
    std::cout << " " << gradient_title() << '\n';
    std::cout << paint(" SYSTEM INTELLIGENCE / BUILD 001 / C++20", Tone::muted) << '\n';
    std::cout << paint(line, Tone::violet) << '\n';
    std::cout << paint("  " + inner, Tone::cyan) << '\n';
    std::cout << paint("  [ ONLINE ]", Tone::mint) << "  "
              << paint("LOCAL DIAGNOSTICS", Tone::muted) << '\n';
    std::cout << '\n';
}

void render_menu() {
    const std::vector<std::string> rows{
        " [01]  SYSTEM//OVERVIEW       Host, kernel, memory, storage",
        " [02]  NETWORK//TOPOLOGY      Local interface addresses",
        " [03]  REPORT//EXPORT         Write a JSON snapshot",
        " [04]  SCREEN//REFRESH        Redraw the command deck",
        " [00]  SESSION//CLOSE        Exit cleanly"
    };

    panel("FILE//SYSTEM :: COMMAND SELECTOR", rows);
    std::cout << paint("  file@cpp ~ $ ", Tone::mint);
}

void panel(const std::string& title, const std::vector<std::string>& lines) {
    const std::size_t width = terminal_width();
    const std::size_t inner = width > 4 ? width - 4 : 60;
    const std::string border(inner, '-');

    std::cout << paint("  +" + border + "+", Tone::cyan) << '\n';
    std::cout << paint("  | ", Tone::cyan)
              << paint(fit(title, inner - 2), Tone::violet)
              << paint(" |", Tone::cyan) << '\n';
    std::cout << paint("  +" + border + "+", Tone::cyan) << '\n';

    for (const auto& line : lines) {
        std::cout << paint("  | ", Tone::cyan)
                  << fit(line, inner - 2)
                  << paint(" |", Tone::cyan) << '\n';
    }

    std::cout << paint("  +" + border + "+", Tone::cyan) << '\n';
}

void status(const std::string& label, const std::string& value, Tone tone) {
    std::cout << "  " << paint("●", tone) << " "
              << paint(label, Tone::muted) << "  "
              << paint(value, tone) << '\n';
}

void notice(const std::string& message, Tone tone) {
    std::cout << "\n  " << paint("◆", tone) << " "
              << paint(message, tone) << '\n';
}

}
