#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

#include "command.h"


class _tr : public COMMAND
{
private:

    vector<fs::path> objects;
    bool fileMode = false;

public:

    _tr() { keyword = "tr"; }

    bool validate(const string& cmd) override {
        stringstream ss(cmd);
        string token;
        ss >> token;

        if (!(ss >> token))
            throw invalid_argument(keyword + ": expected flags (-f or -d)");

        if (token == "-f") fileMode = true;
        else if (token == "-d") fileMode = false;
        else throw invalid_argument(keyword + ": '" + token + "': invalid flag");

        while (ss >> token) {
            if (fileMode && !validate_file_path(token))
                throw invalid_argument(keyword + ": '" + token + "': expected file path");

            if (!fileMode && !validate_dir_path(token))
                throw invalid_argument(keyword + ": '" + token + "': expected directory path");

            objects.push_back(token);
        }

        if (objects.empty())
            throw invalid_argument(keyword + ": missing arguments");

        return true;
    }

    void execute() override {

        for (auto& object : objects) {
            fs::path filepath = get_location(object);

            if (fileMode) {
                if (!fs::exists(filepath) || !fs::is_regular_file(filepath))
                    throw invalid_argument(keyword + ": '" + object.string() + "': no such file");
                ofstream(filepath, ios::trunc).close();
            }

            else {
                if (!fs::exists(filepath) || !fs::is_directory(filepath))
                    throw invalid_argument(keyword + ": '" + object.string() + "': no such directory");

                for (auto& entry : fs::directory_iterator(filepath))
                    fs::remove_all(entry.path());
            }
        }
    }
};
