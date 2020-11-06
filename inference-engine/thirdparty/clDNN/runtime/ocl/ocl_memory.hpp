/*
// Copyright (c) 2016-2021 Intel Corporation
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

#include "ocl_common.hpp"
#include "ocl_engine.hpp"
#include "ocl_stream.hpp"
#include "cldnn/runtime/memory.hpp"

#include <cassert>
#include <iterator>
#include <mutex>
#include <memory>

namespace cldnn {
namespace gpu {
struct lockable_gpu_mem {
    lockable_gpu_mem() :
        _lock_count(0),
        _mapped_ptr(nullptr) {}

    std::mutex _mutex;
    unsigned _lock_count;
    void* _mapped_ptr;
};

struct gpu_buffer : public lockable_gpu_mem, public memory {
    gpu_buffer(ocl_engine* engine, const layout& new_layout, const cl::Buffer& buffer);
    gpu_buffer(ocl_engine* engine, const layout& layout);

    void* lock(const stream& stream) override;
    void unlock(const stream& stream) override;
    event::ptr fill(stream& stream, unsigned char pattern) override;
    event::ptr fill(stream& stream) override;
    shared_mem_params get_internal_params() const override;
    const cl::Buffer& get_buffer() const {
        assert(0 == _lock_count);
        return _buffer;
    }

protected:
    cl::Buffer _buffer;
};

struct gpu_image2d : public lockable_gpu_mem, public memory {
    gpu_image2d(ocl_engine* engine, const layout& new_layout, const cl::Image2D& buffer);
    gpu_image2d(ocl_engine* engine, const layout& layout);

    void* lock(const stream& stream) override;
    void unlock(const stream& stream) override;
    event::ptr fill(stream& stream, unsigned char pattern) override;
    event::ptr fill(stream& stream) override;
    shared_mem_params get_internal_params() const override;
    const cl::Image2D& get_buffer() const {
        assert(0 == _lock_count);
        return _buffer;
    }

protected:
    cl::Image2D _buffer;
    size_t _width;
    size_t _height;
    size_t _row_pitch;
    size_t _slice_pitch;
};

struct gpu_media_buffer : public gpu_image2d {
    gpu_media_buffer(ocl_engine* engine, const layout& new_layout, shared_mem_params params);
    shared_mem_params get_internal_params() const override;
private:
    void* device;
#ifdef _WIN32
    void* surface;
#else
    uint32_t surface;
#endif
    uint32_t plane;
};

#ifdef _WIN32
struct gpu_dx_buffer : public gpu_buffer {
    gpu_dx_buffer(ocl_engine* engine, const layout& new_layout, shared_mem_params VAEncMiscParameterTypeSubMbPartPel);
    shared_mem_params get_internal_params() const override;
private:
    void* device;
    void* resource;
};
#endif

struct gpu_usm : public lockable_gpu_mem, public memory {
    gpu_usm(ocl_engine* engine, const layout& new_layout, const cl::UsmMemory& usm_buffer, allocation_type type);
    gpu_usm(ocl_engine* engine, const layout& layout, allocation_type type);

    void* lock(const stream& stream) override;
    void unlock(const stream& stream) override;
    const cl::UsmMemory& get_buffer() const { return _buffer; }
    cl::UsmMemory& get_buffer() { return _buffer; }

    event::ptr fill(stream& stream, unsigned char pattern) override;
    event::ptr fill(stream& stream) override;
    void copy_from_other(const stream& stream, const memory& other) override;
    shared_mem_params get_internal_params() const override;
protected:
    cl::UsmMemory _buffer;
};
}  // namespace gpu
}  // namespace cldnn
