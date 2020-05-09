
#include "progress_bar.hpp"
#include <platform/platform.h>
#include <cassert>
#include <chrono>
#include <clocale>
#include <cstdio>
#include <ctime>
#include <cwchar>
#include <iostream>
#include <random>
#include <thread>

#if EXP_PLATFORM_OS_IS_WINDOWS
#include <conio.h>
#endif

using namespace std::literals;

int random_number(std::default_random_engine& rg, int min = 3, int max = 10)
{
    std::uniform_int_distribution<int> uniform_dist(min, max);
    return uniform_dist(rg);
}

void show_random_additions(int wait_seconds)
{
    std::random_device rd;
    std::default_random_engine rg(rd());

    for (;;)
    {
        const auto left = random_number(rg);
        const auto right = random_number(rg);
        std::cout << "Combien font : " << left << " + " << right << " ?\n";
        // for (int i = 0; i < wait_seconds; ++i)
        //{
        //    std::cout << "...\n";
        //    std::this_thread::sleep_for(1s);
        //}
        const auto code = [] {
#if EXP_PLATFORM_OS_IS_WINDOWS
            const auto first = _getch();
            if (first != 0)
                return first;
            return _getch();
#else
            return 3;
#endif
        }();
        std::cout << "Reponse : " << left + right << "\n\n";
        //        std::this_thread::sleep_for(1s);
        if (code == 3)
            break;
    }
}

void show_random_multiplications(int wait_seconds)
{
    std::random_device rd;
    std::default_random_engine rg(rd());

    for (;;)
    {
        const auto left = random_number(rg, 0, 5);
        const auto right = random_number(rg, 0, 5);
        std::cout << "Combien font : " << left << " x " << right << " ?\n";
        for (int i = 0; i < wait_seconds; ++i)
        {
            std::cout << "...\n";
            std::this_thread::sleep_for(1s);
        }
        // const auto code = []
        //{
        //    const auto first = _getch();
        //    if (first != 0)
        //        return first;
        //    return _getch();
        //}();
        std::cout << "Reponse : " << left * right << "\n\n";
        //        std::this_thread::sleep_for(std::chrono::seconds(1));
        // if (code == 3)
        //    break;
    }
}

char random_letter(std::default_random_engine& rg)
{
    std::uniform_int_distribution<int> uniform_dist('A', 'Z');
    return uniform_dist(rg);
}

void show_boggle()
{
    std::random_device rd;
    std::default_random_engine rg(rd());

    for (int ligne = 0; ligne < 6; ++ligne)
    {
        for (int colonne = 0; colonne < 6; ++colonne)
        {
            char letter = random_letter(rg);
            std::cout << letter << " ";
        }
        std::cout << "\n\n";
    }
}

void test_utf8()
{
    const auto en_us = std::setlocale(LC_ALL, "en_US.UTF-8");
    assert(en_us);

    const auto fr_fr = std::setlocale(LC_NUMERIC, "fr_FR.UTF-8");
    assert(fr_fr);

    const auto previous_loc = std::setlocale(LC_ALL, nullptr);
    if (previous_loc)
        std::cout << "Current locale is '" << previous_loc << "'\n";

    const auto current_loc = std::setlocale(LC_ALL, "fr_FR.UTF-8");
    if (current_loc)
    {
        std::cout << "Switched to UTF-8 (current locale is now '" << current_loc << "')\n";
        std::cout << "Infinity: \\u221E='\u221E'\n";
    }
    else
        std::cout << "Not able to switch to UTF-8\n";

    std::setlocale(LC_ALL, "en_US.UTF-8");
    std::setlocale(LC_NUMERIC, "de_DE.UTF-8");
    std::setlocale(LC_TIME, "ja_JP.UTF-8");

    wchar_t str[100];
    std::time_t t = std::time(nullptr);
    std::wcsftime(str, 100, L"%A %c", std::localtime(&t));
    std::wprintf(L"Number: %.2f\nDate: %Ls\n", 3.14, str);
}

void test_ANSI_escape_codes()
{
    int i, j, n;

    for (i = 0; i < 11; i++)
    {
        for (j = 1; j < 11; j++)
        {
            n = 10 * i + j - 1;
            if (n > 108)
                break;
            //            std::cout << "\033[" << n << "m " << n << "\033[m";
            std::printf("\033[%dm %3d\033[m", n, n);
        }
        //        std::cout << "\n";
        std::printf("\n");
    }

    std::cout << "HELLO\n";
    std::cout << "FOO";
    std::this_thread::sleep_for(1s);
    std::cout << "\x1b[A";
    std::this_thread::sleep_for(1s);
    std::cout << "\r";
    std::this_thread::sleep_for(1s);
    std::cout << "WORLD\n";
    std::this_thread::sleep_for(1s);
    std::cout << "GREAT!\n";
    std::this_thread::sleep_for(1s);
}

void test_progress_bars()
{
    std::random_device rd;
    std::default_random_engine rg(rd());

    std::vector<ProgressBar> bars(5);
    std::vector<std::reference_wrapper<ProgressBar>> pending_bars;
    std::transform(bars.begin(), bars.end(), back_inserter(pending_bars), [](auto& bar) { return std::ref(bar); });

    std::uniform_int_distribution<unsigned> bar_inc_dist(0U, 10);

    while (!pending_bars.empty())
    {
        std::uniform_int_distribution<std::size_t> bar_index_dist(0U, pending_bars.size() - 1);
        const auto bar_index = bar_index_dist(rg);
        const auto bar_inc = static_cast<float>(bar_inc_dist(rg));
        auto& bar = pending_bars[bar_index].get();
        bar.value += bar_inc;
        if (bar.value >= 100.f)
        {
            bar.status = "Done!";
            pending_bars.erase(pending_bars.begin() + bar_index);
        }

        write_progress({10, 5}, bars);

        std::this_thread::sleep_for(100ms);
    }
}

int main()
{
    //    test_ANSI_escape_codes();
    test_progress_bars();
}
