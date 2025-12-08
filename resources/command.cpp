#include "command.h"
#include "def.h"

#include "_ls.h"
#include "_cd.h"
#include "_mk.h"
#include "_rm.h"
#include "_wr.h"
#include "_rd.h"
#include "_tr.h"


unique_ptr<COMMAND> COMMAND::dispatch(const string& cmd) {
    stringstream ss(cmd);
    string token;
    ss >> token;

    if      (token == "ls")     return make_unique<_ls>();
    else if (token == "cd")     return make_unique<_cd>();
    else if (token == "mk")     return make_unique<_mk>();
    else if (token == "rm")     return make_unique<_rm>();
    else if (token == "tr")     return make_unique<_tr>();
    else if (token == "wr")     return make_unique<_wr>();
    else if (token == "rd")     return make_unique<_rd>();
    else                        throw invalid_argument(token + ": invalid command");
}


fs::path COMMAND::get_location(const fs::path& object) {

    // resolve the parent directory of the object relative to current_directory
    fs::path object_parent;
    try {
        // canonical path of the parent directory
        object_parent = fs::canonical(noob.current_directory / object.parent_path());
    }
    catch (const fs::filesystem_error&) {
        throw invalid_argument(keyword + ": '" + object.parent_path().string() + "': invalid parent path");
    }

    // are you going outside the sandbox?
    fs::path relative = object_parent.lexically_relative(noob.home_directory);
    if (!relative.empty() && relative.string().rfind("..", 0) == 0)
        throw invalid_argument(keyword + ": '" + object.string() + "': access outside of Playground denied");

    return object_parent / object.filename();
}
