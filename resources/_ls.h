#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

#include "command.h"

class _ls : public COMMAND
{
private:

    fs::path targetDir = noob.current_directory;

public:

    _ls() { keyword = "ls"; }

    bool validate(const string& cmd) override {
        stringstream ss(cmd);
        string word;

        ss >> word;

        if (!(ss >> word))
            return true;

        targetDir = word;

        if (!regex_match(targetDir.string(), regex(path_dir)))
            throw invalid_argument(keyword + ": invalid directory name '" + targetDir.string() + "'");

        if (ss >> word)
            throw invalid_argument(keyword + ": too many arguments");

        return true;
    }

    void execute() override {
        filesystem::path dirPath;

        dirPath = get_location(targetDir);

        if (!filesystem::exists(dirPath))
            throw runtime_error(keyword + ": directory does not exist: " + targetDir.string());

        if (!filesystem::is_directory(dirPath))
            throw runtime_error(keyword + ": " + dirPath.filename().string() + ": not a directory");

        for (auto& entry : filesystem::directory_iterator(dirPath)) {
            cout << entry.path().filename().string();
            if (filesystem::is_directory(entry))
                cout << '/';
            cout << endl;
        }
    }
};
