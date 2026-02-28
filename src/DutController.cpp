#include "dut_controller/DutController.h"

#include <utility>

namespace dut {

DutController::DutController(std::unique_ptr<ITransport> transport)
    : transport_(std::move(transport)) {}

DutController::~DutController() {
    if (transport_) {
        (void)transport_->Disconnect();
    }
}

Status DutController::Execute(Operation operation) {
    std::scoped_lock lock(mutex_);
    return ExecuteLocked(operation);
}

Status DutController::ClassicTxStart() { return Execute(Operation::ClassicTxStart); }
Status DutController::ClassicTxStop() { return Execute(Operation::ClassicTxStop); }
Status DutController::ClassicRxStart() { return Execute(Operation::ClassicRxStart); }
Status DutController::ClassicRxStop() { return Execute(Operation::ClassicRxStop); }
Status DutController::LeTxStart() { return Execute(Operation::LeTxStart); }
Status DutController::LeTxStop() { return Execute(Operation::LeTxStop); }
Status DutController::LeRxStart() { return Execute(Operation::LeRxStart); }
Status DutController::LeRxStop() { return Execute(Operation::LeRxStop); }

Status DutController::ExecuteLocked(Operation operation) {
    if (!transport_) {
        return Status::Error("transport is null");
    }

    const auto connect_status = transport_->Connect();
    if (!connect_status.ok()) {
        return connect_status;
    }

    return transport_->SendCommand(ToCommand(operation));
}

}  // namespace dut
