#pragma once

#include <iostream>
#include "command.h"

using namespace std;



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

    void execute() override
    {
        char buffer[BLOCK_SIZE];
        ENTRY dir[MAX_ENTRIES];

        // Read directory entries
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
            if (dir[i].inUse && !dir[i].isDirectory && filePath == dir[i].name) {
                fileEntry = &dir[i];
                break;
            }
        }
        if (!fileEntry)
            throw runtime_error(keyword + ": file not found: " + filePath.string());

        if (fileEntry->start == -1 || fileEntry->size == 0) {
            cout << "[empty file]" << endl;
            return;
        }

        // Read the file content
        fstream disk("playground.bin", ios::binary | ios::in);
        disk.seekg(fileEntry->start * BLOCK_SIZE);
        vector<char> fileContent(fileEntry->size);
        disk.read(fileContent.data(), fileEntry->size);
        disk.close();

        cout << string(fileContent.begin(), fileContent.end()) << endl;
    }
};
