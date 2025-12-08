#pragma once

#include <iostream>
#include <cstring>
#include "command.h"

using namespace std;



class _tr : public COMMAND
{
private:

    vector<fs::path> names;
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

            names.push_back(token);
        }

        if (names.empty())
            throw invalid_argument(keyword + ": missing arguments");

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

        for (const auto& name : names) {
            bool found = false;

            for (int i = 0; i < MAX_ENTRIES; i++) {
                if (dir[i].inUse && name == dir[i].name) {
                    if (fileMode && dir[i].isDirectory)
                        throw runtime_error(keyword + ": cannot truncate directory with -f: " + name.string());
                    if (!fileMode && !dir[i].isDirectory)
                        throw runtime_error(keyword + ": cannot truncate file with -d: " + name.string());

                    // reset size/start for truncation
                    dir[i].size = 0;
                    dir[i].start = -1;
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
