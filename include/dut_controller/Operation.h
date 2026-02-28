#pragma once

#include <string_view>

namespace dut {

enum class Operation {
    ClassicTxStart,
    ClassicTxStop,
    ClassicRxStart,
    ClassicRxStop,
    LeTxStart,
    LeTxStop,
    LeRxStart,
    LeRxStop,
};

inline constexpr std::string_view ToCommand(Operation op) {
    switch (op) {
        case Operation::ClassicTxStart:
            return "classic_tx_start";
        case Operation::ClassicTxStop:
            return "classic_tx_stop";
        case Operation::ClassicRxStart:
            return "classic_rx_start";
        case Operation::ClassicRxStop:
            return "classic_rx_stop";
        case Operation::LeTxStart:
            return "le_tx_start";
        case Operation::LeTxStop:
            return "le_tx_stop";
        case Operation::LeRxStart:
            return "le_rx_start";
        case Operation::LeRxStop:
            return "le_rx_stop";
    }

    return "unknown";
}

}  // namespace dut
