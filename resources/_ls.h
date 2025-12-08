#pragma once

#include <iostream>
#include "command.h"

using namespace std;



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

        if (!regex_match(targetDir.string(), regex(path_dir)) && !regex_match(targetDir.string(), regex("playground.bin")))
            throw invalid_argument(keyword + ": invalid directory name '" + targetDir.string() + "'");

        if (ss >> word)
            throw invalid_argument(keyword + ": too many arguments");

        return true;
    }

    void execute() override {
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

        bool anyPrinted = false;
        for (int i = 0; i < MAX_ENTRIES; i++) {
            if (dir[i].inUse) {
                cout << dir[i].name;
                if (dir[i].isDirectory) cout << '/';
                cout << '\n';
                anyPrinted = true;
            }
        }

        if (!anyPrinted) {
            cout << "(empty directory)" << endl;
        }
    }
};
