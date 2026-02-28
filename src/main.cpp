#include <iostream>

#include "dut_controller/DutController.h"
#include "dut_controller/TransportConfig.h"
#include "dut_controller/TransportFactory.h"

int main() {
    dut::TransportConfig config;
    config.type = dut::TransportType::Uart;
    config.port_name = "COM5";
    config.baud_rate = 921600;

    auto transport = dut::TransportFactory::Create(config);
    dut::DutController controller(std::move(transport));

    const auto status = controller.ClassicTxStart();
    if (!status.ok()) {
        std::cerr << "Failed to start classic TX: " << status.message() << '\n';
        return 1;
    }

    return 0;
}
