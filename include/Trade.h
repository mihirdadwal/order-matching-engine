#pragma once

// ============================================================================
// Trade.h – Represents an executed transaction between a buyer and seller
//
// LEARNING NOTES (C++ vs Java):
// 1. 'inline': Used on functions defined in headers to prevent duplicate
//    symbol linker errors when included in multiple .cpp files.
// 2. 'const' member functions: e.g. 'double toDollars() const'. The 'const' at
//    the end guarantees this function does not modify any member variables.
// ============================================================================

#include "Order.h"
#include <string>
#include <iomanip>
#include <sstream>

struct Trade {
    std::string symbol;
    OrderId buyOrderId;
    OrderId sellOrderId;
    Price price;         // Executed price (in cents)
    Quantity quantity;   // Number of matched shares

    Trade(std::string symbol,
          OrderId buyOrderId,
          OrderId sellOrderId,
          Price price,
          Quantity quantity)
        : symbol(std::move(symbol)),
          buyOrderId(buyOrderId),
          sellOrderId(sellOrderId),
          price(price),
          quantity(quantity) {}

    // Convert price in cents to a formatted dollars string, e.g. 15025 -> "$150.25"
    std::string priceString() const {
        std::ostringstream oss;
        oss << "$" << (price / 100) << "."
            << std::setw(2) << std::setfill('0') << (price % 100);
        return oss.str();
    }
};
