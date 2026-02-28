#include "dut_controller/TransportFactory.h"

#include <memory>
#include <stdexcept>

#include "dut_controller/transports/AdbTransport.h"
#include "dut_controller/transports/UartTransport.h"
#include "dut_controller/transports/UsbTransport.h"

namespace dut {

std::unique_ptr<ITransport> TransportFactory::Create(const TransportConfig& config) {
    switch (config.type) {
        case TransportType::Uart:
            return std::make_unique<UartTransport>(config.port_name, config.baud_rate);
        case TransportType::Adb:
            return std::make_unique<AdbTransport>(config.adb_serial, config.adb_path,
                                                  config.adb_timeout_ms);
        case TransportType::Usb:
            return std::make_unique<UsbTransport>(config.vendor_id, config.product_id);
    }

    throw std::invalid_argument("unsupported transport type");
}

}  // namespace dut
