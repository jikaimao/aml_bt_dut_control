#include "dut_controller/transports/AdbTransport.h"

#include <iostream>
#include <string>
#include <utility>

namespace dut {

AdbTransport::AdbTransport(std::string serial) : serial_(std::move(serial)) {}

Status AdbTransport::Connect() {
    if (connected_) {
        return Status::Ok();
    }

    std::cout << "[ADB] Connect to device"
              << (serial_.empty() ? " (default)" : " serial=" + serial_) << '\n';

    connected_ = true;
    return Status::Ok();
}

Status AdbTransport::Disconnect() {
    connected_ = false;
    return Status::Ok();
}

Status AdbTransport::SendCommand(std::string_view command) {
    if (!connected_) {
        return Status::Error("ADB transport is not connected");
    }

    std::cout << "[ADB] shell " << command << '\n';
    return Status::Ok();
}

}  // namespace dut
