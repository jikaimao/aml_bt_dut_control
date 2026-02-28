#pragma once

#include <string_view>

#include "dut_controller/Status.h"

namespace dut {

class ITransport {
public:
    virtual ~ITransport() = default;

    virtual Status Connect() = 0;
    virtual Status Disconnect() = 0;
    virtual Status SendCommand(std::string_view command) = 0;
};

}  // namespace dut
