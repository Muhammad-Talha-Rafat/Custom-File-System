#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

#include "command.h"



class _mk : public COMMAND
{
private:

    bool isFile = false;
    vector<fs::path> names;
    fs::path base;

public:

    _mk() { keyword = "mk"; }

    bool validate(const string& cmd) override
    {
        stringstream ss(cmd);
        string token;
        vector<string> tokens;

        while (ss >> token)
            tokens.push_back(token);

        if (tokens.size() < 3) throw invalid_argument("Usage: mk -[f|d] name1 name2 ...");

        if (tokens[1] == "-f") isFile = true;
        else if (tokens[1] == "-d") isFile = false;
        else throw invalid_argument(keyword + "invalid flag usage [" + tokens[1] + "]");

        for (int i = 2; i < tokens.size(); i++) {
            if (isFile) {
                if (!regex_match(tokens[i], regex(path_file)))
                    throw invalid_argument(keyword + ": invalid file name '" + tokens[i] + "'");
            }
            else {
                if (!regex_match(tokens[i], regex(path_dir)))
                    throw invalid_argument(keyword + ": invalid directory name '" + tokens[i] + "'");
            }
            names.push_back(tokens[i]);
        }
        return true;
    }

    

    void execute() override
    {
        for (const auto& name : names)
        {
            fs::path fullPath = get_location(name);

            if (isFile) {
                if (fs::exists(fullPath))
                    throw runtime_error(keyword + "File already exists: " + name.string());
                ofstream file(fullPath);
            }
            else {
                if (fs::exists(fullPath))
                    throw runtime_error(keyword + "Directory already exists: " + name.string());
                fs::create_directory(fullPath);
            }
        }
    }
};
