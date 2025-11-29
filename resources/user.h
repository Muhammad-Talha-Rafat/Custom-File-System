#pragma once
#include <filesystem>
using namespace std;
extern struct USER {
    const filesystem::path home_directory = "E:/Kachra/OS Labs/Custom-File-System/Playground";
    filesystem::path current_directory;
    USER() : current_directory(home_directory) {}
} noob;