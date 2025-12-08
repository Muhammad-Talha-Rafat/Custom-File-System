#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>

#include "def.h"

using namespace std;

namespace fs = filesystem;

#include "user.h"

class _cd;
class _mk;
class _rm;
class _ls;
class _tr;
class _wr;
class _rd;


class COMMAND
{
protected:

    string keyword;
    fs::path get_location(const fs::path& object);

    const string path_dir = R"((?:\.\./|\./|/)?(?:[A-Za-z_][A-Za-z0-9_]*|\.\.?)(?:/(?:[A-Za-z_][A-Za-z0-9_]*|\.\.?))*/?)";
    const string path_file = R"((?:(?:\.\./|\./|/)?(?:[A-Za-z_][A-Za-z0-9_]*|\.\.?)(?:/(?:[A-Za-z_][A-Za-z0-9_]*|\.\.?))*/)?(([A-Za-z][A-Za-z0-9_]*|\*))\.[A-Za-z]+)";

    const regex dir_regex{ path_dir };
    const regex file_regex{ path_file };

    const bool validate_dir_path(const fs::path& path) {
        return regex_match(path.string(), dir_regex);
    }

    const bool validate_file_path(const fs::path& path) {
        return regex_match(path.string(), file_regex);
    }

public:

    virtual bool validate(const string& cmd) = 0;
    virtual void execute() = 0;

    static unique_ptr<COMMAND> dispatch(const string& cmd);
};



