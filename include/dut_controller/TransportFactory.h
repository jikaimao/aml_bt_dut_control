#pragma once

#include <memory>

#include "dut_controller/ITransport.h"
#include "dut_controller/TransportConfig.h"

namespace dut {

class TransportFactory {
public:
    static std::unique_ptr<ITransport> Create(const TransportConfig& config);
};

}  // namespace dut
