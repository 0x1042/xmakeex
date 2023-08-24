#include <iostream>

#include <CLI/CLI.hpp>
#include <folly/ScopeGuard.h>
#include <folly/futures/Future.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "cli/cli.h"
#include "infra/resource_manager.h"

auto main(int argc, char ** argv) -> int {
    google::InitGoogleLogging(argv[0]);
    //	google::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_stderrthreshold = google::GLOG_INFO;

    cli::Option opt;
    cli::initCli(argc, argv, opt);

    LOG(INFO) << "host: " << opt.host << " port:" << opt.port;

    auto guard = folly::makeGuard([] {
        LOG(WARNING) << "shutdown ...";
        google::ShutdownGoogleLogging();
    });

    LOG(INFO) << "Hello World";

    ResourceManager::initInstance();

    std::vector<folly::SemiFuture<int>> futures;

    return 0;
}
