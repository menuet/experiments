
#include "delaysubtitles.hpp"
#include <string>


int main(int argc, char* argv[])
{
    if (argc < 3)
        return 1;

    const auto delay = std::stoi(argv[1]);
    const auto* const sourcePath = argv[2];
    const auto* const targetPath = (argc >= 4) ? argv[3] : "";

    delaysubtitles::delaySubTitles(std::chrono::milliseconds(delay), sourcePath, targetPath);

    return 0;
}
