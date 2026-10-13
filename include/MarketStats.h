#pragma once

// ============================================================================
// MarketStats.h – Tracks real-time trading statistics for a symbol
//
// LEARNING NOTES (Financial Domain Terms):
// 1. Last Price: The price at which the most recent trade occurred.
// 2. Volume: Total number of shares executed since startup.
// 3. Spread: The difference between the Lowest Ask and Highest Bid (Ask - Bid).
// 4. VWAP (Volume-Weighted Average Price):
//    Sum(trade_price * trade_quantity) / Total_Volume.
//    Used by institutional traders to measure execution quality.
// ============================================================================

#include "Trade.h"
#include <string>
#include <optional>

class MarketStats {
public:
    MarketStats() = default;

    // Updates statistics with a newly executed trade
    void recordTrade(const Trade& trade);

    // Getters
    Price getLastPrice() const { return lastPrice_; }
    Quantity getTotalVolume() const { return totalVolume_; }

    // VWAP in cents (or nullopt if no trades executed yet)
    std::optional<Price> getVWAP() const;

    // Formatted helper strings
    std::string lastPriceString() const;
    std::string vwapString() const;

private:
    Price lastPrice_{0};
    Quantity totalVolume_{0};
    int64_t totalTurnoverCents_{0}; // Sum of (price * quantity)
};
