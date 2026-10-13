#pragma once

// ============================================================================
// Exchange.h – Top-level coordinator managing multiple stock Order Books
//
// LEARNING NOTES (Architecture):
// 1. Single Responsibility: OrderBook manages matching for 1 symbol. Exchange
//    manages the universe of symbols (AAPL, TSLA, MSFT).
// 2. Dynamic Symbol Registration: If an order arrives for a symbol that hasn't
//    been traded yet, Exchange creates its OrderBook on demand.
// ============================================================================

#include "OrderBook.h"
#include "MarketStats.h"
#include <string>
#include <unordered_map>
#include <vector>

class Exchange {
public:
    Exchange() = default;

    // Submits an order to the appropriate symbol's order book
    std::vector<Trade> submitOrder(Order order);

    // Cancels an order globally by its ID across all symbol order books
    bool cancelOrder(OrderId orderId);

    // Access order book for a symbol (creates one if not already present)
    OrderBook& getOrCreateBook(const std::string& symbol);

    // Const access to order book (returns nullptr if not found)
    const OrderBook* getBook(const std::string& symbol) const;

    // Const access to market statistics for a symbol
    const MarketStats* getStats(const std::string& symbol) const;

    // Returns a list of all active symbols
    std::vector<std::string> getActiveSymbols() const;

private:
    std::unordered_map<std::string, OrderBook> books_;
    std::unordered_map<std::string, MarketStats> stats_;
    std::unordered_map<OrderId, std::string> orderToSymbolMap_;
};
