// ============================================================================
// Display.cpp – ANSI colored terminal formatting
// ============================================================================

#include "Display.h"
#include <iostream>
#include <iomanip>

namespace Colors {
    const char* RESET   = "\033[0m";
    const char* BOLD    = "\033[1m";
    const char* RED     = "\033[31m";
    const char* GREEN   = "\033[32m";
    const char* YELLOW  = "\033[33m";
    const char* CYAN    = "\033[36m";
    const char* WHITE   = "\033[37m";
}

void Display::printOrderBook(const OrderBook& book, const MarketStats& stats) {
    auto bestBid = book.getBestBid();
    auto bestAsk = book.getBestAsk();

    std::string spreadStr = "--";
    if (bestBid.has_value() && bestAsk.has_value() && *bestAsk >= *bestBid) {
        Price spreadCents = *bestAsk - *bestBid;
        std::ostringstream oss;
        oss << "$" << (spreadCents / 100) << "."
            << std::setw(2) << std::setfill('0') << (spreadCents % 100);
        spreadStr = oss.str();
    }

    std::cout << "\n" << Colors::BOLD << Colors::CYAN
              << "========================== " << book.getSymbol() << " MARKET =========================="
              << Colors::RESET << "\n";

    // Statistics Dashboard Header
    std::cout << Colors::BOLD
              << "  Last: " << Colors::YELLOW << stats.lastPriceString() << Colors::RESET
              << "   Volume: " << Colors::WHITE << stats.getTotalVolume() << Colors::RESET
              << "   VWAP: " << Colors::CYAN << stats.vwapString() << Colors::RESET
              << "   Spread: " << Colors::WHITE << spreadStr << Colors::RESET << "\n";
    std::cout << "------------------------------------------------------------\n";

    // ASKS (Sellers) – RED
    std::cout << Colors::BOLD << Colors::RED << "  ASKS (Sellers)" << Colors::RESET << "\n";
    const auto& asks = book.getAsks();
    if (asks.empty()) {
        std::cout << "    (no resting asks)\n";
    } else {
        // Highest ask down to lowest (best ask is closest to the middle)
        for (auto it = asks.rbegin(); it != asks.rend(); ++it) {
            Price price = it->first;
            Quantity totalQty = 0;
            for (const auto& o : it->second) totalQty += o.remainingQuantity;

            double dollarPrice = price / 100.0;
            std::cout << Colors::RED
                      << "    $" << std::fixed << std::setprecision(2) << std::setw(8) << dollarPrice
                      << "  |  " << std::setw(6) << totalQty << " shares (" << it->second.size() << " orders)"
                      << Colors::RESET << "\n";
        }
    }

    std::cout << "------------------------------------------------------------\n";

    // BIDS (Buyers) – GREEN
    std::cout << Colors::BOLD << Colors::GREEN << "  BIDS (Buyers)" << Colors::RESET << "\n";
    const auto& bids = book.getBids();
    if (bids.empty()) {
        std::cout << "    (no resting bids)\n";
    } else {
        // Highest bid (best bid) down to lowest
        for (const auto& [price, queue] : bids) {
            Quantity totalQty = 0;
            for (const auto& o : queue) totalQty += o.remainingQuantity;

            double dollarPrice = price / 100.0;
            std::cout << Colors::GREEN
                      << "    $" << std::fixed << std::setprecision(2) << std::setw(8) << dollarPrice
                      << "  |  " << std::setw(6) << totalQty << " shares (" << queue.size() << " orders)"
                      << Colors::RESET << "\n";
        }
    }

    std::cout << Colors::BOLD << Colors::CYAN
              << "============================================================"
              << Colors::RESET << "\n\n";
}

void Display::printTrade(const Trade& trade) {
    std::cout << Colors::BOLD << Colors::YELLOW
              << "TRADE [" << trade.symbol << "]: "
              << Colors::RESET
              << trade.quantity << " shares @ " << trade.priceString()
              << " (Buyer #" << trade.buyOrderId << ", Seller #" << trade.sellOrderId << ")\n";
}
