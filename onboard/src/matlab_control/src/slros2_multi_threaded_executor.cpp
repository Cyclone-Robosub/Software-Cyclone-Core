// Copyright 2024 The MathWorks, Inc.

// Copyright 2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 *  Copyright (C) 2024 The MathWorks, Inc.
 *  MathWorks-specific modifications have been made to the original source.
 */

#include "slros2_multi_threaded_executor.h"
#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include "rcpputils/scope_exit.hpp"

#include "rclcpp/logging.hpp"
#include "rclcpp/utilities.hpp"

using rclcpp::executors::SLMultiThreadedExecutor;

SLMultiThreadedExecutor::SLMultiThreadedExecutor(
  const rclcpp::ExecutorOptions & options,
  size_t number_of_threads,
  bool yield_before_execute,
  std::chrono::nanoseconds next_exec_timeout)
: rclcpp::Executor(options),
  yield_before_execute_(yield_before_execute),
  next_exec_timeout_(next_exec_timeout)
{
  number_of_threads_ = number_of_threads > 0 ?
    number_of_threads :
    std::max(std::thread::hardware_concurrency(), 2U);

  if (number_of_threads_ == 1) {
    RCLCPP_WARN(
      rclcpp::get_logger("rclcpp"),
      "MultiThreadedExecutor is used with a single thread.\n"
      "Use the SingleThreadedExecutor instead.");
  }
}

SLMultiThreadedExecutor::~SLMultiThreadedExecutor() {}

void SLMultiThreadedExecutor::stopSubscriberCallback(rclcpp::SubscriptionBase* subscriber)
{
  //Add the subscriber to the list
  skipped_subscribers_.insert(subscriber);
}

void
SLMultiThreadedExecutor::spin()
{
  if (spinning.exchange(true)) {
    throw std::runtime_error("spin() called while already spinning");
  }
  
  #ifdef ROS2_DISTRO_JAZZY //in jazzy
    RCPPUTILS_SCOPE_EXIT(wait_result_.reset();this->spinning.store(false););
  #else //before jazzy
    RCPPUTILS_SCOPE_EXIT(this->spinning.store(false); );
  #endif
  
  std::vector<std::thread> threads;
  size_t thread_id = 0;
  {
    std::lock_guard wait_lock{wait_mutex_};
    for (; thread_id < number_of_threads_ - 1; ++thread_id) {
      auto func = std::bind(&SLMultiThreadedExecutor::run, this, thread_id);
      threads.emplace_back(func);
    }
  }

  run(thread_id);
  for (auto & thread : threads) {
    thread.join();
  }
}

size_t
SLMultiThreadedExecutor::get_number_of_threads()
{
  return number_of_threads_;
}

void
SLMultiThreadedExecutor::run(size_t this_thread_number)
{
  (void)this_thread_number;
  while (rclcpp::ok(this->context_) && spinning.load()) {
    rclcpp::AnyExecutable any_exec;
    {
      std::lock_guard wait_lock{wait_mutex_};
      if (!rclcpp::ok(this->context_) || !spinning.load()) {
        return;
      }
      
      // If nothing to be executed or if it is the subscriber from the simulink model, skip it.
      if (!get_next_executable(any_exec, next_exec_timeout_) || 
      	(any_exec.subscription && (skipped_subscribers_.find(any_exec.subscription.get()) != skipped_subscribers_.end()))){
        continue;
      }
    }
    if (yield_before_execute_) {
      std::this_thread::yield();
    }

    execute_any_executable(any_exec);

    // From jazzy
   #ifdef ROS2_DISTRO_JAZZY
    if (any_exec.callback_group &&
      any_exec.callback_group->type() == CallbackGroupType::MutuallyExclusive)
    {
      try {
        interrupt_guard_condition_->trigger();
      } catch (const rclcpp::exceptions::RCLError & ex) {
        throw std::runtime_error(
                std::string(
                  "Failed to trigger guard condition on callback group change: ") + ex.what());
      }
    }
   #endif
   
    // Clear the callback_group to prevent the AnyExecutable destructor from
    // resetting the callback group `can_be_taken_from`
    any_exec.callback_group.reset();
  }
}
