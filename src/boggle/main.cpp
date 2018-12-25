
#include <random>
#include <iostream>
#include <thread>
#include <chrono>
#include <conio.h>

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
        //for (int i = 0; i < wait_seconds; ++i)
        //{
        //    std::cout << "...\n";
        //    std::this_thread::sleep_for(std::chrono::seconds(1));
        //}
        const auto code = []
        {
            const auto first = _getch();
            if (first != 0)
                return first;
            return _getch();
        }();
        std::cout << "Reponse : " << left + right << "\n\n";
//        std::this_thread::sleep_for(std::chrono::seconds(1));
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
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        //const auto code = []
        //{
        //    const auto first = _getch();
        //    if (first != 0)
        //        return first;
        //    return _getch();
        //}();
        std::cout << "Reponse : " << left * right << "\n\n";
        //        std::this_thread::sleep_for(std::chrono::seconds(1));
        //if (code == 3)
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

int main()
{
    show_random_multiplications(3);
}
