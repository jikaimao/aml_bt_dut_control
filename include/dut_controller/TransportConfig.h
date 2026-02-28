#pragma once

#include <cstdint>
#include <string>

#include "dut_controller/TransportTypes.h"

namespace dut {

struct TransportConfig {
    TransportType type {TransportType::Uart};

    // UART
    std::string port_name {"COM1"};
    std::uint32_t baud_rate {115200};

    // ADB
    std::string adb_serial {};

    // USB
    std::uint16_t vendor_id {0};
    std::uint16_t product_id {0};
};

}  // namespace dut
