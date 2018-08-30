
#include <random>
#include <iostream>

char random_letter(std::default_random_engine& rg)
{
    std::uniform_int_distribution<int> uniform_dist('A', 'Z');
    return uniform_dist(rg);
}

int main()
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