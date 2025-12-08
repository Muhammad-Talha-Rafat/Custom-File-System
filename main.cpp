#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;

#include "resources\user.h"
#include "resources\command.h"

#define BLOCK_SIZE 1024
#define DISK_SIZE 64 * 1024 * 1024
#define MAX_ENTRIES 128
#define FAT_FREE -1
#define FAT_END -2

const int TOTAL_BLOCKS = DISK_SIZE / BLOCK_SIZE;

struct ENTRY {
    char name[64];
    bool isDirectory;
    int  start;
    int  size;
    bool inUse;
};


void WriteBlock(int block_num, char* buffer) {
    fstream disk("playground.bin", ios::binary | ios::out);
    disk.seekp(block_num * BLOCK_SIZE);
    disk.write(buffer, BLOCK_SIZE);
    disk.close();
}

void ReadBlock(int block_num, char* buffer) {
    fstream disk("playground.bin", ios::binary | ios::in);
    disk.seekg(block_num * BLOCK_SIZE);
    disk.read(buffer, BLOCK_SIZE);
    disk.close();
}


void formatDisk() {

    ofstream disk("playground.bin", ios::binary);
    disk.seekp(DISK_SIZE - 1);
    disk.write("", 1);
    disk.close();

    int FAT_SIZE_BYTES = TOTAL_BLOCKS * sizeof(int);
    int FAT_BLOCKS = (FAT_SIZE_BYTES + BLOCK_SIZE - 1) / BLOCK_SIZE;

    cout << "Total Blocks: " << TOTAL_BLOCKS << "\n";
    cout << "FAT Blocks:   " << FAT_BLOCKS << "\n";

    vector<int> FAT(TOTAL_BLOCKS);

    for (int i = 0; i < TOTAL_BLOCKS; i++)
        FAT[i] = FAT_FREE;

    for (int i = 0; i < FAT_BLOCKS; i++)
        FAT[i] = FAT_END;

    char buffer[BLOCK_SIZE];

    int cursor = 0;
    for (int i = 0; i < FAT_BLOCKS; i++) {
        memset(buffer, 0, BLOCK_SIZE);
        memcpy(buffer, FAT.data() + (i * BLOCK_SIZE / sizeof(int)), BLOCK_SIZE);
        WriteBlock(1 + i, buffer);
        cursor += BLOCK_SIZE;
    }

    ENTRY dir[MAX_ENTRIES];
    for (int i = 0; i < MAX_ENTRIES; i++) {
        dir[i].inUse = false;
        dir[i].isDirectory = false;
        dir[i].name[0] = '\0';
        dir[i].size = 0;
        dir[i].start = -1;
    }

    int DIR_BLOCKS = 10;
    int DIR_START_BLOCK = 1 + FAT_BLOCKS;

    cursor = 0;
    for (int i = 0; i < DIR_BLOCKS; i++) {
        memset(buffer, 0, BLOCK_SIZE);
        memcpy(buffer, &dir[cursor], BLOCK_SIZE);
        WriteBlock(DIR_START_BLOCK + i, buffer);
        cursor += BLOCK_SIZE / sizeof(ENTRY);
    }

    cout << "Disk format: successful\n";
    cout << "FAT starts at block 1\n";
    cout << "Session starts at block " << DIR_START_BLOCK << "\n";
}


USER noob;


int main() {
    formatDisk();

    while (true) {

        string cmd;
        string noob_location = noob.current_directory.string().substr(37);
        cout << "\033[0;32m" << noob_location << " $ " << "\033[0m";
        getline(cin, cmd);

        if (cmd == "exit") exit(0);
        else if (cmd == "clear") system("clear");
        else if (cmd == "format") formatDisk();
        else try {
            auto command = COMMAND::dispatch(cmd);
            command->validate(cmd);
            command->execute();
        }
        catch (const exception& e) {
            cerr << "\033[0;31m" << e.what() << "\033[0m" << endl;
            return false;
        }

        

    }
}