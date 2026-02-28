#pragma once

#include <string>

namespace dut {

class Status {
public:
    static Status Ok() { return Status(true, {}); }

    static Status Error(std::string message) {
        return Status(false, std::move(message));
    }

    [[nodiscard]] bool ok() const noexcept { return ok_; }
    [[nodiscard]] const std::string& message() const noexcept { return message_; }

private:
    Status(bool ok, std::string message) : ok_(ok), message_(std::move(message)) {}

    bool ok_;
    std::string message_;
};

}  // namespace dut
