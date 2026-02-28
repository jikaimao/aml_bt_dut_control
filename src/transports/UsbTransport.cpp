#include "dut_controller/transports/UsbTransport.h"

#include <iomanip>
#include <iostream>

namespace dut {

UsbTransport::UsbTransport(std::uint16_t vendor_id, std::uint16_t product_id)
    : vendor_id_(vendor_id), product_id_(product_id) {}

Status UsbTransport::Connect() {
    if (connected_) {
        return Status::Ok();
    }

    if (vendor_id_ == 0 || product_id_ == 0) {
        return Status::Error("USB VID/PID must be configured");
    }

    std::cout << "[USB] Connect to VID:PID " << std::hex << std::setfill('0') << std::setw(4)
              << vendor_id_ << ":" << std::setw(4) << product_id_ << std::dec << '\n';

    connected_ = true;
    return Status::Ok();
}

Status UsbTransport::Disconnect() {
    connected_ = false;
    return Status::Ok();
}

Status UsbTransport::SendCommand(std::string_view command) {
    if (!connected_) {
        return Status::Error("USB transport is not connected");
    }

    std::cout << "[USB] Send: " << command << '\n';
    return Status::Ok();
}

}  // namespace dut
