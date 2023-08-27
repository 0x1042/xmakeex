#include <format>
#include <iostream>
#include <source_location>

#include <CLI/CLI.hpp>
#include <folly/ScopeGuard.h>
#include <folly/futures/Future.h>
#include <spdlog/spdlog.h>

#include "cli/cli.h"
#include "infra/resource_manager.h"
#include "socks/server.h"

auto main(int argc, char ** argv) -> int {
    google::InitGoogleLogging(argv[0]);
    FLAGS_stderrthreshold = google::GLOG_INFO;
    spdlog::set_level(spdlog::level::trace);

    cli::Option opt;
    cli::initCli(argc, argv, opt);

    LOG(INFO) << "host: " << opt.host << " port:" << opt.port;

    auto loc = std::source_location::current();

    std::cout << "line:" << loc.file_name() << " " << loc.function_name() << " " << loc.line() << '\n';

    auto guard = folly::makeGuard([] {
        LOG(WARNING) << "shutdown ...";
        google::ShutdownGoogleLogging();
    });

    LOG(INFO) << "Hello World";

    ResourceManager::initInstance();

    socks::serve(10086);

    return 0;
}
