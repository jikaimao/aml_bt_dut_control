#include "dut_controller/transports/UsbTransport.h"

namespace dut {

UsbTransport::UsbTransport(std::uint16_t vendor_id, std::uint16_t product_id)
    : vendor_id_(vendor_id), product_id_(product_id) {}

Status UsbTransport::Connect() {
    if (vendor_id_ == 0 || product_id_ == 0) {
        return Status::Error("USB VID/PID must be configured");
    }
    return Status::Error("USB transport is reserved for project-specific implementation");
}

Status UsbTransport::Disconnect() {
    connected_ = false;
    return Status::Ok();
}

Status UsbTransport::SendCommand(std::string_view command) {
    (void)command;
    return Status::Error("USB transport is reserved for project-specific implementation");
}

}  // namespace dut
