#pragma once

#include <string>

#include "dut_controller/ITransport.h"

namespace dut {

class AdbTransport final : public ITransport {
public:
    explicit AdbTransport(std::string serial);

    Status Connect() override;
    Status Disconnect() override;
    Status SendCommand(std::string_view command) override;

private:
    std::string serial_;
    bool connected_ {false};
};

}  // namespace dut
