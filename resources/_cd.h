#pragma once

#include <iostream>
#include "command.h"

using namespace std;



class _cd : public COMMAND
{
private:

    fs::path directory = "";

public:

    _cd() { keyword = "cd"; }

    bool validate(const string& cmd) override {
        stringstream ss(cmd);
        string token;
        ss >> token;

        if (ss >> token) {
            if (validate_dir_path(token))
                directory = token;
            else throw invalid_argument(keyword + " '" + token + "': expected a directory");
            if (ss >> token)
                throw invalid_argument(keyword + ": '" + token + "': too many arguments");
        }
        return true;
    }

    void execute() override {
        if (directory.empty()) noob.current_directory = noob.home_directory;
        else try {
            fs::path canonical_path = fs::canonical(noob.current_directory / directory);
            if (canonical_path.lexically_relative(noob.home_directory).string().rfind("..", 0) == 0)
                throw invalid_argument(keyword + ": (error) out of bounds: access denied");

            noob.current_directory = canonical_path;
        }
        catch (const fs::filesystem_error& e) {
            throw invalid_argument(keyword + ": '" + directory.string() + "': no such directory or path");
        }
    }
};
