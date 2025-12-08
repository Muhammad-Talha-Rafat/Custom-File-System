#pragma once

#include <iostream>
#include <cstring>
#include "command.h"

using namespace std;



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
            bool exists = false;
            int freeIndex = -1;

            for (int i = 0; i < MAX_ENTRIES; i++) {
                if (dir[i].inUse && name == dir[i].name) {
                    exists = true;
                    break;
                }
                if (!dir[i].inUse && freeIndex == -1)
                    freeIndex = i;
            }

            if (exists)
                throw runtime_error(keyword + ": already exists: " + name.string());

            if (freeIndex == -1)
                throw runtime_error(keyword + ": no free directory entry");

            dir[freeIndex].inUse = true;
            dir[freeIndex].isDirectory = !isFile;
            dir[freeIndex].size = 0;
            dir[freeIndex].start = -1;
            strncpy(dir[freeIndex].name, name.string().c_str(), sizeof(dir[freeIndex].name) - 1);
            dir[freeIndex].name[sizeof(dir[freeIndex].name) - 1] = '\0';
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
