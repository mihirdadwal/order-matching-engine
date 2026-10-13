// ============================================================================
// Exchange.cpp – Implementation of Multi-Symbol Exchange
// ============================================================================

#include "Exchange.h"

OrderBook& Exchange::getOrCreateBook(const std::string& symbol) {
    auto it = books_.find(symbol);
    if (it == books_.end()) {
        auto [insertedIt, success] = books_.emplace(symbol, OrderBook(symbol));
        // Initialize stats as well
        stats_.emplace(symbol, MarketStats());
        return insertedIt->second;
    }
    return it->second;
}

const OrderBook* Exchange::getBook(const std::string& symbol) const {
    auto it = books_.find(symbol);
    if (it == books_.end()) return nullptr;
    return &(it->second);
}

const MarketStats* Exchange::getStats(const std::string& symbol) const {
    auto it = stats_.find(symbol);
    if (it == stats_.end()) return nullptr;
    return &(it->second);
}

std::vector<Trade> Exchange::submitOrder(Order order) {
    std::string symbol = order.symbol;
    OrderId id = order.id;

    OrderBook& book = getOrCreateBook(symbol);
    auto trades = book.addOrder(order);

    // Track order ID -> symbol for cancel routing
    orderToSymbolMap_[id] = symbol;

    // Update market statistics for any generated trades
    MarketStats& stat = stats_[symbol];
    for (const auto& trade : trades) {
        stat.recordTrade(trade);
    }

    return trades;
}

bool Exchange::cancelOrder(OrderId orderId) {
    auto it = orderToSymbolMap_.find(orderId);
    if (it == orderToSymbolMap_.end()) {
        return false;
    }

    std::string symbol = it->second;
    orderToSymbolMap_.erase(it);

    auto bookIt = books_.find(symbol);
    if (bookIt != books_.end()) {
        return bookIt->second.cancelOrder(orderId);
    }

    return false;
}

std::vector<std::string> Exchange::getActiveSymbols() const {
    std::vector<std::string> symbols;
    symbols.reserve(books_.size());
    for (const auto& [symbol, book] : books_) {
        symbols.push_back(symbol);
    }
    return symbols;
}
