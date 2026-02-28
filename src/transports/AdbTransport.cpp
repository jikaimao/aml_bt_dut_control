#include "dut_controller/transports/AdbTransport.h"

#include <sstream>
#include <string>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#endif

namespace dut {

namespace {
#ifdef _WIN32
std::string QuoteArg(const std::string& arg) {
    if (arg.find(' ') == std::string::npos && arg.find('"') == std::string::npos) {
        return arg;
    }

    std::string escaped = "\"";
    for (const char c : arg) {
        if (c == '"') {
            escaped += "\\\"";
        } else {
            escaped.push_back(c);
        }
    }
    escaped += '"';
    return escaped;
}

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

AdbTransport::AdbTransport(std::string serial, std::string adb_path, std::uint32_t timeout_ms)
    : serial_(std::move(serial)), adb_path_(std::move(adb_path)), timeout_ms_(timeout_ms) {}

Status AdbTransport::Connect() {
    if (connected_) {
        return Status::Ok();
    }

#ifdef _WIN32
    std::string output;
    Status status = ExecuteAdb("start-server", &output);
    if (!status.ok()) {
        return status;
    }

    if (!serial_.empty()) {
        status = ExecuteAdb("-s " + QuoteArg(serial_) + " get-state", &output);
        if (!status.ok()) {
            return status;
        }
    }

    connected_ = true;
    return Status::Ok();
#else
    return Status::Error("ADB transport supports Windows only");
#endif
}

Status AdbTransport::Disconnect() {
    connected_ = false;
    return Status::Ok();
}

Status AdbTransport::SendCommand(std::string_view command) {
    if (!connected_) {
        return Status::Error("ADB transport is not connected");
    }

#ifdef _WIN32
    std::ostringstream args;
    if (!serial_.empty()) {
        args << "-s " << QuoteArg(serial_) << ' ';
    }
    args << "shell " << QuoteArg(std::string(command));

    std::string output;
    return ExecuteAdb(args.str(), &output);
#else
    (void)command;
    return Status::Error("ADB transport supports Windows only");
#endif
}

Status AdbTransport::ExecuteAdb(const std::string& args, std::string* output) {
#ifdef _WIN32
    if (adb_path_.empty()) {
        return Status::Error("ADB executable path cannot be empty");
    }

    SECURITY_ATTRIBUTES sa {};
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = nullptr;

    HANDLE read_pipe = nullptr;
    HANDLE write_pipe = nullptr;
    if (!CreatePipe(&read_pipe, &write_pipe, &sa, 0)) {
        return Status::Error("CreatePipe failed: " + GetWin32ErrorMessage(GetLastError()));
    }

    if (!SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(read_pipe);
        CloseHandle(write_pipe);
        return Status::Error("SetHandleInformation failed: " +
                             GetWin32ErrorMessage(GetLastError()));
    }

    STARTUPINFOA startup_info {};
    startup_info.cb = sizeof(STARTUPINFOA);
    startup_info.dwFlags = STARTF_USESTDHANDLES;
    startup_info.hStdOutput = write_pipe;
    startup_info.hStdError = write_pipe;

    PROCESS_INFORMATION process_info {};

    std::string command_line = QuoteArg(adb_path_) + " " + args;
    std::string mutable_command = command_line;

    const BOOL created = CreateProcessA(nullptr, mutable_command.data(), nullptr, nullptr, TRUE,
                                        CREATE_NO_WINDOW, nullptr, nullptr, &startup_info,
                                        &process_info);

    CloseHandle(write_pipe);

    if (!created) {
        CloseHandle(read_pipe);
        return Status::Error("CreateProcessA failed: " + GetWin32ErrorMessage(GetLastError()) +
                             " (command: " + command_line + ")");
    }

    const DWORD wait_result = WaitForSingleObject(process_info.hProcess, timeout_ms_);
    if (wait_result == WAIT_TIMEOUT) {
        TerminateProcess(process_info.hProcess, 1);
        CloseHandle(process_info.hThread);
        CloseHandle(process_info.hProcess);
        CloseHandle(read_pipe);
        return Status::Error("ADB command timeout: " + command_line);
    }

    std::string local_output;
    char buffer[512] {};
    DWORD bytes_read = 0;
    while (ReadFile(read_pipe, buffer, sizeof(buffer), &bytes_read, nullptr) && bytes_read > 0) {
        local_output.append(buffer, bytes_read);
    }

    DWORD exit_code = 1;
    if (!GetExitCodeProcess(process_info.hProcess, &exit_code)) {
        exit_code = 1;
    }

    CloseHandle(process_info.hThread);
    CloseHandle(process_info.hProcess);
    CloseHandle(read_pipe);

    if (output != nullptr) {
        *output = local_output;
    }

    if (exit_code != 0) {
        return Status::Error("ADB command failed (exit=" + std::to_string(exit_code) + "): " +
                             command_line + "\n" + local_output);
    }

    return Status::Ok();
#else
    (void)args;
    (void)output;
    return Status::Error("ADB transport supports Windows only");
#endif
}

}  // namespace dut
