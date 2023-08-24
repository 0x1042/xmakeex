//
// Created by 韦轩 on 2023/8/24.
//

#pragma once
#include <memory>

#include <folly/executors/CPUThreadPoolExecutor.h>

class ResourceManager {
private:
    ResourceManager() = default;

public:
    ~ResourceManager() = default;
    ResourceManager(ResourceManager &&) = delete;
    auto operator=(ResourceManager &&) -> ResourceManager & = delete;
    ResourceManager(ResourceManager &) = delete;
    auto operator=(ResourceManager &) -> ResourceManager & = delete;

    static auto instance() -> ResourceManager & { return *instance_; }

    static auto initInstance() -> int;

private:
    static std::unique_ptr<ResourceManager> instance_;
    auto init() -> int;

    static auto createExecutor(size_t num, const std::string & prefix = "exec.", bool block = false)
        -> std::shared_ptr<folly::CPUThreadPoolExecutor>;

public:
    std::shared_ptr<folly::CPUThreadPoolExecutor> higt_exec;
};
