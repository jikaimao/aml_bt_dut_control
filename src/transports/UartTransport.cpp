#include "dut_controller/transports/UartTransport.h"

#include <string>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#endif

namespace dut {

namespace {
#ifdef _WIN32
std::string GetWin32ErrorMessage(const DWORD error_code) {
    LPSTR buffer = nullptr;
    const DWORD length = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                            FORMAT_MESSAGE_IGNORE_INSERTS,
                                        nullptr, error_code,
                                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                        reinterpret_cast<LPSTR>(&buffer), 0, nullptr);

    if (length == 0 || buffer == nullptr) {
        return "unknown Win32 error";
    }

    std::string message(buffer, length);
    LocalFree(buffer);
    return message;
}
#endif
}  // namespace

UartTransport::UartTransport(std::string port_name, unsigned int baud_rate)
    : port_name_(NormalizePortName(port_name)), baud_rate_(baud_rate) {}

UartTransport::~UartTransport() { (void)Disconnect(); }

Status UartTransport::Connect() {
    if (connected_) {
        return Status::Ok();
    }

#ifdef _WIN32
    if (port_name_.empty()) {
        return Status::Error("UART port name cannot be empty");
    }

    if (baud_rate_ == 0) {
        return Status::Error("UART baud rate must be greater than zero");
    }

    const std::string device_name = "\\\\.\\" + port_name_;
    HANDLE handle = CreateFileA(device_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (handle == INVALID_HANDLE_VALUE) {
        return Status::Error("UART open failed: " + GetWin32ErrorMessage(GetLastError()));
    }

    handle_ = handle;
    const Status config_status = ConfigurePort();
    if (!config_status.ok()) {
        (void)Disconnect();
        return config_status;
    }

    connected_ = true;
    return Status::Ok();
#else
    return Status::Error("UART transport supports Windows only");
#endif
}

Status UartTransport::Disconnect() {
#ifdef _WIN32
    if (handle_ != nullptr) {
        CloseHandle(static_cast<HANDLE>(handle_));
        handle_ = nullptr;
    }
#endif
    connected_ = false;
    return Status::Ok();
}

Status UartTransport::SendCommand(std::string_view command) {
    if (!connected_) {
        return Status::Error("UART transport is not connected");
    }

#ifdef _WIN32
    std::string payload(command);
    payload.push_back('\n');
    const DWORD payload_size = static_cast<DWORD>(payload.size());

    DWORD written = 0;
    const BOOL ok = WriteFile(static_cast<HANDLE>(handle_), payload.data(), payload_size, &written,
                              nullptr);
    if (!ok || written != payload_size) {
        return Status::Error("UART write failed: " + GetWin32ErrorMessage(GetLastError()));
    }
    return Status::Ok();
#else
    (void)command;
    return Status::Error("UART transport supports Windows only");
#endif
}

Status UartTransport::ConfigurePort() {
#ifdef _WIN32
    DCB dcb {};
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(static_cast<HANDLE>(handle_), &dcb)) {
        return Status::Error("UART GetCommState failed: " + GetWin32ErrorMessage(GetLastError()));
    }

    dcb.BaudRate = baud_rate_;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fBinary = TRUE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;

    if (!SetCommState(static_cast<HANDLE>(handle_), &dcb)) {
        return Status::Error("UART SetCommState failed: " + GetWin32ErrorMessage(GetLastError()));
    }

    COMMTIMEOUTS timeouts {};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(static_cast<HANDLE>(handle_), &timeouts)) {
        return Status::Error("UART SetCommTimeouts failed: " + GetWin32ErrorMessage(GetLastError()));
    }

    if (!PurgeComm(static_cast<HANDLE>(handle_), PURGE_RXCLEAR | PURGE_TXCLEAR)) {
        return Status::Error("UART PurgeComm failed: " + GetWin32ErrorMessage(GetLastError()));
    }

    return Status::Ok();
#else
    return Status::Error("UART transport supports Windows only");
#endif
}

std::string UartTransport::NormalizePortName(const std::string& port_name) {
    if (port_name.rfind("\\\\.\\", 0) == 0) {
        return port_name.substr(4);
    }
    return port_name;
}

}  // namespace dut
