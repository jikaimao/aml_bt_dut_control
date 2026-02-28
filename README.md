# aml_bt_dut_control

Windows 平台 C++ DUT 控制框架（工业化可扩展设计），支持 UART / ADB / USB 等多传输方式，并对外提供统一 DUT 控制接口。

## 设计目标

- **统一业务接口**：对外只暴露 `classic_tx_start/stop`、`classic_rx_start/stop`、`le_tx_start/stop`、`le_rx_start/stop` 等能力。
- **可扩展传输层**：通过 `ITransport` 抽象解耦业务命令和通信介质，后续可新增 SPI/TCP/自定义驱动。
- **工业级基础能力**：
  - 面向接口编程（`IDutController` / `ITransport`）
  - 工厂模式（`TransportFactory`）
  - 线程安全控制（`DutController` 内部互斥锁）
  - 清晰状态返回（`Status`）
- **构建体系**：CMake 管理，兼容 MinGW32 + `g++ 15.1.0`。

## 当前实现状态

- ✅ **UART（Windows 实现）**：基于 Win32 串口 API（`CreateFileA / GetCommState / SetCommState / WriteFile`）实现连接、配置与命令发送。
- ✅ **ADB（Windows 实现）**：基于 Win32 进程 API（`CreateProcessA` + 管道）执行 `adb` 命令，支持序列号、可配置超时与错误输出收集。
- 🧩 **USB（预留）**：保留接口与工厂接入点，你可以按项目硬件方案替换为自定义实现。

## 目录结构

```text
include/dut_controller/
  IDutController.h
  DutController.h
  ITransport.h
  TransportFactory.h
  TransportConfig.h
  Operation.h
  Status.h
  transports/
    UartTransport.h
    AdbTransport.h
    UsbTransport.h

src/
  DutController.cpp
  TransportFactory.cpp
  transports/
    UartTransport.cpp
    AdbTransport.cpp
    UsbTransport.cpp
  main.cpp
```

## 快速构建（Windows + MinGW）

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++
cmake --build build -j
```

> 如果在非 Windows 主机验证，可直接用默认生成器：

```bash
cmake -S . -B build
cmake --build build -j
```

## 统一接口示例

```cpp
dut::TransportConfig config;
config.type = dut::TransportType::Uart;
config.port_name = "COM5";
config.baud_rate = 921600;

auto transport = dut::TransportFactory::Create(config);
dut::DutController controller(std::move(transport));

const auto status = controller.ClassicTxStart();
if (!status.ok()) {
    // 错误处理
}
```

## ADB 配置示例

```cpp
dut::TransportConfig config;
config.type = dut::TransportType::Adb;
config.adb_path = "C:/platform-tools/adb.exe";
config.adb_serial = "0123456789ABCDEF"; // 可选
config.adb_timeout_ms = 15000;
```

## 扩展新命令

1. 在 `Operation` 枚举中新增操作。
2. 在 `ToCommand()` 中映射到底层命令字符串。
3. 在 `IDutController`/`DutController` 添加便捷函数（可选）。

## 扩展新通信方式

1. 新建 `XxxTransport : public ITransport`。
2. 实现 `Connect/Disconnect/SendCommand`。
3. 在 `TransportFactory::Create` 注册创建逻辑。
4. 在 `TransportConfig` 添加所需配置项。
