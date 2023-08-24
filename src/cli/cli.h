#pragma once

#include <string>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#ifndef VERSION
#    define VERSION unknown
#endif

namespace cli {

struct Option {
    uint16_t port{0};
    std::string host;
};

auto initCli(int argc, char ** argv, Option & opt) -> int;
} // namespace cli
