#pragma once

#include "dut_controller/Operation.h"
#include "dut_controller/Status.h"

namespace dut {

class IDutController {
public:
    virtual ~IDutController() = default;

    virtual Status Execute(Operation operation) = 0;

    virtual Status ClassicTxStart() = 0;
    virtual Status ClassicTxStop() = 0;
    virtual Status ClassicRxStart() = 0;
    virtual Status ClassicRxStop() = 0;
    virtual Status LeTxStart() = 0;
    virtual Status LeTxStop() = 0;
    virtual Status LeRxStart() = 0;
    virtual Status LeRxStop() = 0;
};

}  // namespace dut
