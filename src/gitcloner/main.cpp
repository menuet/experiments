
#include <platform/filesystem.hpp>
#include <platform/platform.h>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

struct GithubUser
{
    std::string name;
    std::string account;

    std::string name_without_spaces() const
    {
        std::string result;
        std::transform(name.begin(), name.end(), back_inserter(result),
                       [](char c) { return std::isspace(c) ? '_' : c; });
        return result;
    }

    std::string user_url() const
    {
        return "https://github.com/" + account;
    }

    std::string repo_url(const std::string& repo_name) const
    {
        return user_url() + "/" + repo_name + ".git";
    }

    stdnext::filesystem::path clone_dir(const stdnext::filesystem::path& root_dir) const
    {
        return root_dir / (name_without_spaces() + "-" + account);
    }
};

std::vector<GithubUser> load_users(const stdnext::filesystem::path& users_file_path)
{
    std::vector<GithubUser> users;

    std::ifstream users_stream{users_file_path.string()};

    std::string user_line;
    while (std::getline(users_stream, user_line))
    {
        std::istringstream iss{user_line};
        GithubUser user;
        std::getline(iss, user.name, ';');
        std::getline(iss, user.account, ';');
        users.push_back(std::move(user));
    }

    return users;
}

void clone_users_repositories(const stdnext::filesystem::path& root_dir, const std::string& repo_name,
                              const std::vector<GithubUser>& users)
{
    std::ofstream vscode_workspace_stream{(root_dir / (repo_name + ".code-workspace")).string()};

    vscode_workspace_stream << "{\n";
    vscode_workspace_stream << "\t\"folders\": [";

    bool more_than_one = false;

    for (const auto& user : users)
    {
        if (user.account.empty())
        {
            std::cout << "Skipping " << user.name << "'s repo (no account yet)\n";
        }
        else
        {
            const auto clone_dir = user.clone_dir(root_dir);
            if (stdnext::filesystem::exists(clone_dir))
            {
                std::cout << "Pulling " << user.name << "'s repo - Executing: ";
                std::stringstream ss;
                ss << "git pull --rebase";
                std::cout << ss.str() << "\n";
                stdnext::filesystem::current_path(clone_dir);
                std::system(ss.str().c_str());
            }
            else
            {
                std::cout << "Cloning " << user.name << "'s repo - Executing: ";
                std::stringstream ss;
                ss << "git clone \"" << user.repo_url(repo_name) << "\" \"" << clone_dir.string() << "\"";
                std::cout << ss.str() << "\n";
                std::system(ss.str().c_str());
            }
            if (!more_than_one)
                more_than_one = true;
            else
                vscode_workspace_stream << ",";
            vscode_workspace_stream << "\n\t\t{\n";
            vscode_workspace_stream << "\t\t\t\"path\": \"" << clone_dir.filename().string() << "\"\n";
            vscode_workspace_stream << "\t\t}";
        }
    }

    vscode_workspace_stream << "\n\t]\n";
    vscode_workspace_stream << "}\n";
}

int main(int argc, char** argv)
{
    const char* users_file_path = (argc > 1) ? argv[1] : R"(D:\DEV\PERSO\TRAINING\EFREI\eleves\names_accounts.txt)";
    const char* root_dir = (argc > 2) ? argv[2] : R"(D:\DEV\PERSO\TRAINING\EFREI\eleves)";
    const char* repo_name = (argc > 3) ? argv[3] : "bulls_and_cows_skeleton";

    const auto users = load_users(users_file_path);

    clone_users_repositories(root_dir, repo_name, users);

    return 0;
}
