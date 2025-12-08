#pragma once

#include <iostream>
#include <cstring>
#include "command.h"

using namespace std;



class _wr : public COMMAND
{
private:

    string text;
    fs::path fileName;
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

        ss >> fileName;

        if (!regex_match(fileName.string(), regex(path_file)))
            throw invalid_argument(keyword + ": invalid file name '" + fileName.string() + "'");

        return true;
    }

        void execute() override
    {
        char buffer[BLOCK_SIZE];
        ENTRY dir[MAX_ENTRIES];

        // Read all directory entries
        int entriesPerBlock = BLOCK_SIZE / sizeof(ENTRY);
        int totalRead = 0;
        for (int i = 0; i < DIR_BLOCKS && totalRead < MAX_ENTRIES; i++) {
            ReadBlock(DIR_START_BLOCK + i, buffer);
            int copyCount = min(entriesPerBlock, MAX_ENTRIES - totalRead);
            memcpy(&dir[totalRead], buffer, copyCount * sizeof(ENTRY));
            totalRead += copyCount;
        }

        // Find the file entry
        ENTRY* fileEntry = nullptr;
        for (int i = 0; i < MAX_ENTRIES; i++) {
            if (dir[i].inUse && !dir[i].isDirectory && fileName == dir[i].name) {
                fileEntry = &dir[i];
                break;
            }
        }
        if (!fileEntry)
            throw runtime_error(keyword + ": file not found: " + fileName.string());

        // Allocate start block if empty
        if (fileEntry->start == -1) fileEntry->start = FAT_START_BLOCK + DIR_BLOCKS;

        // Calculate write offset
        int startBlock = fileEntry->start;
        int writeOffset = redirection ? fileEntry->size : 0;

        // Open disk
        fstream disk("playground.bin", ios::binary | ios::in | ios::out);
        if (!disk.is_open())
            throw runtime_error(keyword + ": failed to open disk");

        // Seek to the file start
        disk.seekp(startBlock * BLOCK_SIZE + writeOffset);
        disk.write(text.c_str(), text.size());
        disk.close();

        // Update file size
        fileEntry->size = redirection ? fileEntry->size + text.size() : text.size();

        // Write back directory entries
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
