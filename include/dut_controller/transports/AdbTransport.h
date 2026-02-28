#pragma once

#include <cstdint>
#include <string>

#include "dut_controller/ITransport.h"

namespace dut {

class AdbTransport final : public ITransport {
public:
    AdbTransport(std::string serial, std::string adb_path, std::uint32_t timeout_ms);

    Status Connect() override;
    Status Disconnect() override;
    Status SendCommand(std::string_view command) override;

private:
    Status ExecuteAdb(const std::string& args, std::string* output);

    std::string serial_;
    std::string adb_path_;
    std::uint32_t timeout_ms_;
    bool connected_ {false};
};

}  // namespace dut
