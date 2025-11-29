#pragma once
using namespace std;


#include "_ls.h"


class _cd;
class _mkd;
class _mkf;
class _rmd;
class _rmf;
class _ls;
class _trunc;
class _wr;
class _rd;
class _format;



class COMMAND
{
protected:

    string keyword;

public:

    virtual bool validate(const string& cmd) = 0;
    virtual void execute() = 0;

    static unique_ptr<COMMAND> dispatch(const string& cmd);
};



unique_ptr<COMMAND> COMMAND::dispatch(const string& cmd) {
    stringstream ss(cmd);
    string token;
    ss >> token;

    if      (token == "ls")      return make_unique<_ls>(token);
    else if (token == "cd")      return make_unique<cd_CMD>(token);
    else if (token == "mkdir")   return make_unique<mkdir_CMD>(token);
    else if (token == "rmdir")   return make_unique<rmdir_CMD>(token);
    else if (token == "rm")      return make_unique<rm_CMD>(token);
    else                         throw invalid_argument(token + ": invalid command");
}