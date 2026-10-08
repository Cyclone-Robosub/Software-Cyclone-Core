// Copyright 2024 The MathWorks, Inc.

// Copyright 2014 Open Source Robotics Foundation, Inc.
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
 
#ifndef RCLCPP__EXECUTORS__SL_MULTI_THREADED_EXECUTOR_HPP_
#define RCLCPP__EXECUTORS__SL_MULTI_THREADED_EXECUTOR_HPP_

#include <chrono>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include "rclcpp/executor.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp/memory_strategies.hpp"
#include "rclcpp/visibility_control.hpp"

namespace rclcpp
{
namespace executors
{

class SLMultiThreadedExecutor : public rclcpp::Executor
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(SLMultiThreadedExecutor)

  /// Constructor for SLMultiThreadedExecutor.
  explicit SLMultiThreadedExecutor(
    const rclcpp::ExecutorOptions & options = rclcpp::ExecutorOptions(),
    size_t number_of_threads = 2,
    bool yield_before_execute = false,
    std::chrono::nanoseconds timeout = std::chrono::nanoseconds(-1));

  virtual ~SLMultiThreadedExecutor();

  /**
   * \sa rclcpp::Executor:spin() for more details
   * \throws std::runtime_error when spin() called while already spinning
   */
  void
  spin() override;

  size_t
  get_number_of_threads();
  
  /*
   * The subscribers sent to this function will not be executed by the executors.
   * This is used in simulink generated code to stop executing the callback of 
   * subscribers available in the model 
   */
  void stopSubscriberCallback(rclcpp::SubscriptionBase*);

protected:
  void
  run(size_t this_thread_number);

private:
  RCLCPP_DISABLE_COPY(SLMultiThreadedExecutor)

  std::mutex wait_mutex_;
  size_t number_of_threads_;
  bool yield_before_execute_;
  std::chrono::nanoseconds next_exec_timeout_;
  
  //List of subscribers that will not be executed by the executor
  std::unordered_set<rclcpp::SubscriptionBase*> skipped_subscribers_;
};

}  // namespace executors
}  // namespace rclcpp

#endif  // RCLCPP__EXECUTORS__SL_MULTI_THREADED_EXECUTOR_HPP_
