
#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>


struct ProgressBar
{
    float value{0.f};
    size_t width{50};
    std::string fill{"#"};
    std::string remainder{" "};
    std::string status{""};
};

struct Location
{
    int x;
    int y;
};

inline void write_progress(Location location, const ProgressBar& bar, std::ostream& os = std::cout)
{
    const auto progress_value = std::min(bar.value, 100.0f);

    // Move cursor to the first position on the same line and flush
    // os << "\r" << std::flush;
    os << "\x1b[" << location.y << ";" << location.x << "H";

    // Start bar
    os << "[";

    const auto completed = static_cast<size_t>(progress_value * static_cast<float>(bar.width) / 100.0);
    for (size_t i = 0; i < completed; ++i)
        os << bar.fill;
    for (size_t i = completed; i < bar.width; ++i)
        os << bar.remainder;

    // End bar
    os << "]";

    // Write progress percentage
    os << " " << std::min(static_cast<size_t>(progress_value), size_t(100)) << "%";

    // Write status text
    os << " " << bar.status;
}

inline void write_progress(Location location, const std::vector<ProgressBar>& bars, std::ostream& os = std::cout)
{
    for (const auto& bar : bars)
    {
        write_progress(location, bar, os);
        ++location.y;
        os << "\n";
    }

    // for (size_t i = 0; i < bars.size(); ++i)
    //     os << "\x1b[A";
}
