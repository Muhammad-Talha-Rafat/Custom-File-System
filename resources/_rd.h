#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

#include "command.h"



class _rd : public COMMAND
{
private:

    fs::path filePath;

public:

    _rd() { keyword = "rd"; }

    bool validate(const string& cmd) override {
        stringstream ss(cmd);
        string token;

        ss >> token;

        if (!(ss >> token))
            throw invalid_argument(keyword + ": missing file argument");

        if (!validate_file_path(token))
            throw invalid_argument(keyword + ": '" + token + "': expected a file");

        filePath = token;

        if (ss >> token)
            throw invalid_argument(keyword + ": '" + token + "': too many arguments");

        return true;
    }

    void execute() override {

        filePath = get_location(filePath);

        if (!fs::exists(filePath))
            throw invalid_argument(keyword + ": '" + filePath.string() + "': no such file");

        if (!fs::is_regular_file(filePath))
            throw invalid_argument(keyword + ": '" + filePath.string() + "': not a file");

        ifstream in(filePath);

        if (!in.is_open())
            throw runtime_error(keyword + ": '" + filePath.string() + "': unable to open file");

        string line;
        while (getline(in, line))
            cout << line << "\n";
    }
};
