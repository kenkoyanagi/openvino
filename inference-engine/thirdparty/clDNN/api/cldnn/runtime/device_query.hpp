// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "device.hpp"
#include "engine_configuration.hpp"

#include <map>
#include <string>
#include <algorithm>

namespace cldnn {

struct device_query {
public:
    explicit device_query(runtime_types runtime_type, void* user_context = nullptr, void* user_device = nullptr);

    std::map<std::string, device::ptr> get_available_devices() const {
        return _available_devices;
    }

    ~device_query() = default;
private:
    std::map<std::string, device::ptr> _available_devices;
};
}  // namespace cldnn
