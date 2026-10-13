// ============================================================================
// MarketStats.cpp – Implementation of Market Statistics calculations
// ============================================================================

#include "MarketStats.h"
#include <iomanip>
#include <sstream>

void MarketStats::recordTrade(const Trade& trade) {
    lastPrice_ = trade.price;
    totalVolume_ += trade.quantity;
    totalTurnoverCents_ += static_cast<int64_t>(trade.price) * trade.quantity;
}

std::optional<Price> MarketStats::getVWAP() const {
    if (totalVolume_ == 0) return std::nullopt;
    return static_cast<Price>(totalTurnoverCents_ / totalVolume_);
}

std::string MarketStats::lastPriceString() const {
    if (totalVolume_ == 0) return "--";
    std::ostringstream oss;
    oss << "$" << (lastPrice_ / 100) << "."
        << std::setw(2) << std::setfill('0') << (lastPrice_ % 100);
    return oss.str();
}

std::string MarketStats::vwapString() const {
    auto vwap = getVWAP();
    if (!vwap.has_value()) return "--";
    std::ostringstream oss;
    oss << "$" << (*vwap / 100) << "."
        << std::setw(2) << std::setfill('0') << (*vwap % 100);
    return oss.str();
}
