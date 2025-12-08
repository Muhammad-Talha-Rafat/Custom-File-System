#pragma once

#include <iostream>
#include <cstring>

using namespace std;

#include "command.h"


class _rm : public COMMAND
{
private:

    bool isFile = false;
    vector<fs::path> targets;

public:

    _rm() { keyword = "rm"; }

    bool validate(const string& cmd) override {

        stringstream ss(cmd);
        string word;
        ss >> word;

        ss >> word;
        if (word == "-f") isFile = true;
        else if (word == "-d") isFile = false;
        else throw invalid_argument(keyword + ": invalid flag usage [" + word + "]");

        while (ss >> word) {
            if (isFile) {
                if (!regex_match(word, regex(path_file)))
                    throw invalid_argument(keyword + ": invalid file name '" + word + "'");
            }
            else if (!regex_match(word, regex(path_dir)))
                    throw invalid_argument(keyword + ": invalid directory name '" + word + "'");
            targets.push_back(word);
        }

        return true;
    }

    void execute() override
    {
        char buffer[BLOCK_SIZE];
        ENTRY dir[MAX_ENTRIES];

        int entriesPerBlock = BLOCK_SIZE / sizeof(ENTRY);
        int totalRead = 0;
        for (int i = 0; i < DIR_BLOCKS && totalRead < MAX_ENTRIES; i++) {
            ReadBlock(DIR_START_BLOCK + i, buffer);
            int copyCount = min(entriesPerBlock, MAX_ENTRIES - totalRead);
            memcpy(&dir[totalRead], buffer, copyCount * sizeof(ENTRY));
            totalRead += copyCount;
        }

        for (const auto& name : targets) {
            bool found = false;

            for (int i = 0; i < MAX_ENTRIES; i++) {
                if (dir[i].inUse && name == dir[i].name) {
                    if (isFile && dir[i].isDirectory)
                        throw runtime_error(keyword + ": cannot remove directory with -f: " + name.string());
                    if (!isFile && !dir[i].isDirectory)
                        throw runtime_error(keyword + ": cannot remove file with -d: " + name.string());

                    dir[i].inUse = false; // mark as removed
                    found = true;
                    break;
                }
            }

            if (!found)
                throw runtime_error(keyword + ": not found: " + name.string());
        }

        int written = 0;
        for (int i = 0; i < DIR_BLOCKS && written < MAX_ENTRIES; i++) {
            memset(buffer, 0, BLOCK_SIZE);
            int copyCount = min(entriesPerBlock, MAX_ENTRIES - written);
            memcpy(buffer, &dir[written], copyCount * sizeof(ENTRY));
            WriteBlock(DIR_START_BLOCK + i, buffer);
            written += copyCount;
        }
    }
};
