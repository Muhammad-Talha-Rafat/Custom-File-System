#pragma once

#include <iostream>
#include "command.h"

using namespace std;



class _rm : public COMMAND
{
private:

    bool removeFiles = false;
    vector<fs::path> targets;

public:

    _rm() { keyword = "rm"; }

    bool validate(const string& cmd) override {

        stringstream ss(cmd);
        string word;
        ss >> word;

        ss >> word;
        if (word == "-f") removeFiles = true;
        else if (word == "-d") removeFiles = false;
        else throw invalid_argument(keyword + ": invalid flag usage [" + word + "]");

        while (ss >> word) {
            if (removeFiles) {
                if (!regex_match(word, regex(path_file)))
                    throw invalid_argument(keyword + ": invalid file name '" + word + "'");
            }
            else if (!regex_match(word, regex(path_dir)))
                    throw invalid_argument(keyword + ": invalid directory name '" + word + "'");
            targets.push_back(word);
        }

        return true;
    }

    void execute() override {

        for (const auto& name : targets) {
            filesystem::path fullPath = get_location(name);

            if (!fs::exists(fullPath))
                throw runtime_error(keyword + ": file does not exist: " + name.string());

            if (removeFiles)
                filesystem::remove(fullPath);
            else if (filesystem::is_directory(fullPath))
                filesystem::remove_all(fullPath);
        }
    }
};
