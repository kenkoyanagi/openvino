/*
// Copyright (c) 2019-2021 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "cldnn/runtime/event.hpp"
#include "cldnn/runtime/stream.hpp"
#include "sycl_common.hpp"
#include "sycl_engine.hpp"

#include <memory>
#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

namespace cldnn {
namespace sycl {

class events_pool;

class sycl_stream : public stream {
public:
    cl::sycl::queue& queue() { return _command_queue; }

    explicit sycl_stream(const sycl_engine& engine);
    sycl_stream(sycl_stream&& other)
        : _engine(other._engine),
          _command_queue(other._command_queue),
          _queue_counter(other._queue_counter.load()),
          _last_barrier(other._last_barrier.load()),
          _events_pool(std::move(other._events_pool)),
          _last_barrier_ev(other._last_barrier_ev),
          _output_event(other._output_event) {}

    ~sycl_stream() = default;

    void sync_events(std::vector<event::ptr> const& deps, bool is_output_event = false) override;
    void release_pending_memory();
    void flush() override;
    void finish() override;

    void set_output_event(bool out_event) { _output_event = out_event; }

    void set_arguments(kernel& kernel, const kernel_arguments_desc& args_desc, const kernel_arguments_data& args) override;
    event::ptr enqueue_kernel(kernel& kernel,
                              const kernel_arguments_desc& args_desc,
                              const kernel_arguments_data& args,
                              std::vector<event::ptr> const& deps,
                              bool is_output_event = false) override;
    event::ptr enqueue_marker(std::vector<event::ptr> const& deps, bool is_output_event) override;
    event::ptr group_events(std::vector<event::ptr> const& deps) override;
    void wait_for_events(const std::vector<event::ptr>& events) override;
    void enqueue_barrier() override;
    void reset_events() override;
    event::ptr create_user_event(bool set) override;
    event::ptr create_base_event() override;
    void release_events_pool() override;

private:
    const sycl_engine& _engine;
    cl::sycl::queue _command_queue;
    std::atomic<uint64_t> _queue_counter{0};
    std::atomic<uint64_t> _last_barrier{0};
    std::shared_ptr<events_pool> _events_pool;
    cl::sycl::event _last_barrier_ev;
    bool _output_event = false;
};

}  // namespace sycl
}  // namespace cldnn
