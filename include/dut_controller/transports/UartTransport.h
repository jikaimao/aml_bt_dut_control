#pragma once

#include <string>

#include "dut_controller/ITransport.h"

namespace dut {

class UartTransport final : public ITransport {
public:
    UartTransport(std::string port_name, unsigned int baud_rate);

    Status Connect() override;
    Status Disconnect() override;
    Status SendCommand(std::string_view command) override;

private:
    std::string port_name_;
    unsigned int baud_rate_;
    bool connected_ {false};
};

}  // namespace dut
