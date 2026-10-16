#pragma once

#include <string>
#include <vector>

namespace aether::ui {

enum class Tone {
    cyan,
    violet,
    magenta,
    mint,
    amber,
    red,
    muted,
    white
};

std::string paint(const std::string& text, Tone tone);
std::string gradient_title();
void clear_screen();
void render_banner();
void render_menu();
void panel(const std::string& title, const std::vector<std::string>& lines);
void status(const std::string& label, const std::string& value, Tone tone);
void notice(const std::string& message, Tone tone);

}
