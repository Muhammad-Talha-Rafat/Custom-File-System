#pragma once

#include "command.h"

class _ls : public COMMAND
{
public:

    bool validate(const string& cmd) override {};

    void execute() override {};

};