#include "dut_controller/transports/UartTransport.h"

#include <iostream>
#include <string>
#include <utility>

namespace dut {

UartTransport::UartTransport(std::string port_name, unsigned int baud_rate)
    : port_name_(std::move(port_name)), baud_rate_(baud_rate) {}

Status UartTransport::Connect() {
    if (connected_) {
        return Status::Ok();
    }

    if (port_name_.empty()) {
        return Status::Error("UART port name cannot be empty");
    }

    std::cout << "[UART] Connect to " << port_name_ << " @ " << baud_rate_ << '\n';
    connected_ = true;
    return Status::Ok();
}

Status UartTransport::Disconnect() {
    connected_ = false;
    return Status::Ok();
}

Status UartTransport::SendCommand(std::string_view command) {
    if (!connected_) {
        return Status::Error("UART transport is not connected");
    }

    std::cout << "[UART] Send: " << command << '\n';
    return Status::Ok();
}

}  // namespace dut
