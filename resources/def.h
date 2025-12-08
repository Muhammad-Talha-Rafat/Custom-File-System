#pragma once
#include <fstream>
using namespace std;

constexpr int BLOCK_SIZE = 1024;
constexpr int DISK_SIZE = 64 * 1024 * 1024;
constexpr int MAX_ENTRIES = 128;
constexpr int FAT_FREE = -1;
constexpr int FAT_END = -2;

constexpr int TOTAL_BLOCKS = DISK_SIZE / BLOCK_SIZE;

struct ENTRY {
    char name[64];
    bool isDirectory;
    int start;
    int size;
    bool inUse;
};

constexpr int DIR_BLOCKS = 10;
constexpr int FAT_START_BLOCK = 1;
constexpr int DIR_START_BLOCK = FAT_START_BLOCK + ((TOTAL_BLOCKS * sizeof(int) + BLOCK_SIZE - 1) / BLOCK_SIZE);


inline void WriteBlock(int block_num, char* buffer) {
    fstream disk("playground.bin", ios::binary | ios::out);
    disk.seekp(block_num * BLOCK_SIZE);
    disk.write(buffer, BLOCK_SIZE);
    disk.close();
}

inline void ReadBlock(int block_num, char* buffer) {
    fstream disk("playground.bin", ios::binary | ios::in);
    disk.seekg(block_num * BLOCK_SIZE);
    disk.read(buffer, BLOCK_SIZE);
    disk.close();
}


void writeDirectory(const ENTRY dir[]) {
    char buffer[BLOCK_SIZE];
    int entriesPerBlock = BLOCK_SIZE / sizeof(ENTRY);
    int written = 0;

    for (int i = 0; i < DIR_BLOCKS && written < MAX_ENTRIES; i++) {
        memset(buffer, 0, BLOCK_SIZE);
        int copyCount = min(entriesPerBlock, MAX_ENTRIES - written);
        memcpy(buffer, &dir[written], copyCount * sizeof(ENTRY));
        WriteBlock(DIR_START_BLOCK + i, buffer);
        written += copyCount;
    }
}
