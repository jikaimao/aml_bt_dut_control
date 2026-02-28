#pragma once

#include <memory>
#include <mutex>

#include "dut_controller/IDutController.h"
#include "dut_controller/ITransport.h"

namespace dut {

class DutController final : public IDutController {
public:
    explicit DutController(std::unique_ptr<ITransport> transport);
    ~DutController() override;

    DutController(const DutController&) = delete;
    DutController& operator=(const DutController&) = delete;

    Status Execute(Operation operation) override;

    Status ClassicTxStart() override;
    Status ClassicTxStop() override;
    Status ClassicRxStart() override;
    Status ClassicRxStop() override;
    Status LeTxStart() override;
    Status LeTxStop() override;
    Status LeRxStart() override;
    Status LeRxStop() override;

private:
    Status ExecuteLocked(Operation operation);

    std::unique_ptr<ITransport> transport_;
    std::mutex mutex_;
};

}  // namespace dut
