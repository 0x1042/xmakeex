//
// Created by 韦轩 on 2023/8/24.
//

#include "resource_manager.h"

#include <cstddef>
#include <memory>

#include "folly/executors/CPUThreadPoolExecutor.h"
#include "folly/executors/task_queue/LifoSemMPMCQueue.h"
#include "glog/logging.h"

std::unique_ptr<ResourceManager> ResourceManager::instance_ = nullptr;

auto ResourceManager::initInstance() -> int {
    instance_.reset(new ResourceManager());
    return instance_->init();
}

auto ResourceManager::init() -> int {
    higt_exec = createExecutor(4, "global");
    return 0;
}

auto ResourceManager::createExecutor(size_t num, const std::string & prefix, bool block)
    -> std::shared_ptr<folly::CPUThreadPoolExecutor> {
    using CPUTask = folly::CPUThreadPoolExecutor::CPUTask;
    using Executor = folly::CPUThreadPoolExecutor;
    size_t queue_size = folly::CPUThreadPoolExecutor::kDefaultMaxQueueSize;

    using THROWQueue = folly::LifoSemMPMCQueue<CPUTask, folly::QueueBehaviorIfFull::THROW>;
    using BLOCKQueue = folly::LifoSemMPMCQueue<CPUTask, folly::QueueBehaviorIfFull::BLOCK>;

    auto factory = std::make_shared<folly::NamedThreadFactory>(prefix);

    LOG(INFO) << "create thread pool. size:" << num << " name:" << prefix << " queue:" << queue_size;
    if (block) {
        return std::make_shared<Executor>(num, std::make_unique<BLOCKQueue>(queue_size), factory);
    }

    return std::make_shared<Executor>(num, std::make_unique<THROWQueue>(queue_size), factory);
}
