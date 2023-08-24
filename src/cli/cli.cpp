#include "cli.h"

#include <CLI/CLI.hpp>

namespace cli {
auto initCli(int argc, char ** argv, Option & opt) -> int {
    CLI::App app{"simple app"};
    uint16_t port{0};
    std::string host;
    app.add_option("-p,--port", opt.port, "bind port");
    app.add_option("--host", opt.host, "bind host");
    app.set_version_flag("--version", TOSTRING(VERSION));

    CLI11_PARSE(app, argc, argv);

    return 0;
}
} // namespace cli
