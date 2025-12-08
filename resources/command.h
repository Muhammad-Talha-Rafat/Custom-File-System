#pragma once
#include <filesystem>
#include <regex>
using namespace std;


#include "_ls.h"
#include "_mk.h"
#include "_rm.h"
#include "_tr.h"
#include "_wr.h"
#include "_rd.h"
#include "_cd.h"

namespace fs = filesystem;

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



unique_ptr<COMMAND> COMMAND::dispatch(const string& cmd) {
    stringstream ss(cmd);
    string token;
    ss >> token;

    if      (token == "ls")     return make_unique<_ls>(token);
    else if (token == "cd")     return make_unique<_cd>(token);
    else if (token == "mk")     return make_unique<_mk>(token);
    else if (token == "rm")     return make_unique<_rm>(token);
    else if (token == "tr")     return make_unique<_tr>(token);
    else if (token == "wr")     return make_unique<_wr>(token);
    else if (token == "rd")     return make_unique<_rd>(token);
    else                        throw invalid_argument(token + ": invalid command");
}


fs::path COMMAND::get_location(const fs::path& object) {

    // try to get the parent directory
    fs::path object_parent;
    try {
        // try to get parent directory
        object_parent = fs::canonical(noob.current_directory / object.parent_path());
    }
    catch(...) {
        throw invalid_argument(keyword + ": '" + object.parent_path().string() + "': bad parent path");
    }

    // throw error if processing location leads beyond Playground
    if (object_parent.lexically_relative(noob.home_directory).string().rfind("..", 0) == 0)
        throw invalid_argument(keyword + ": (out of bounds) access denied");

    return object_parent / object.filename();
}