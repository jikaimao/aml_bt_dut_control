#pragma once

#include <cstdint>

#include "dut_controller/ITransport.h"

namespace dut {

class UsbTransport final : public ITransport {
public:
    UsbTransport(std::uint16_t vendor_id, std::uint16_t product_id);

    Status Connect() override;
    Status Disconnect() override;
    Status SendCommand(std::string_view command) override;

private:
    std::uint16_t vendor_id_;
    std::uint16_t product_id_;
    bool connected_ {false};
};

}  // namespace dut
