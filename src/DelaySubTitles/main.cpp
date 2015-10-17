
#include "delaysubtitles.hpp"
#include <string>


int main(int argc, char* argv[])
{
    if (argc != 4)
        return 1;

    const auto* const sourceFilePath = argv[1];
    const auto* const targetFilePath = argv[2];
    const auto delay = std::stoi(argv[3]);

    delaysubtitles::delaySubTitles(sourceFilePath, targetFilePath, std::chrono::milliseconds(delay));

    return 0;
}
