#pragma once

#include <iostream>
#include "command.h"

using namespace std;



class _wr : public COMMAND
{
private:

    string text;
    fs::path filename;
    bool redirection = false;

public:

    _wr() { keyword = "wr"; }

    bool validate(const string& cmd) override {
        size_t firstQ = cmd.find('"');
        size_t lastQ  = cmd.find_last_of('"');

        if (firstQ == string::npos || lastQ == firstQ)
            throw invalid_argument(keyword + ": invalid syntax, missing quotes");

        text = cmd.substr(firstQ + 1, lastQ - firstQ - 1);

        string after = cmd.substr(lastQ + 1);
        stringstream ss(after);
        string op;
        ss >> op;

        if (op == ">") redirection = false;
        else if (op == ">>") redirection = true;
        else throw invalid_argument(keyword + ": missing redirection operator");

        ss >> filename;

        if (!regex_match(filename.string(), regex(path_file)))
            throw invalid_argument(keyword + ": invalid file name '" + filename.string() + "'");

        return true;
    }


    void execute() override {
        fs::path fullPath = get_location(filename);

        if (!fs::exists(fullPath))
            throw runtime_error(keyword + ": file does not exist: " + filename.string());

        ios_base::openmode mode = ios::out;
        if (redirection) mode |= ios::app;
        else mode |= ios::trunc;

        ofstream file(fullPath, mode);

        file << text;
        file.close();
    }
};
