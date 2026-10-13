#pragma once

// ============================================================================
// Order.h – Defines the data structures for Orders
// 
// LEARNING NOTES (C++ vs Java):
// 1. '#pragma once': Tells the compiler to include this header file only once
//    per compilation unit. Prevents "duplicate definition" errors.
// 2. 'enum class': Strongly typed enums in C++. Unlike regular enums or Java,
//    you must access values using the scope operator (e.g. Side::Buy).
// 3. 'using': Creates a type alias (similar to typedef or TypeScript type aliases).
//    Instead of writing uint64_t everywhere, we write OrderId.
// 4. 'struct': In C++, a struct is identical to a class, except members are
//    PUBLIC by default. We use structs for simple Plain Old Data (POD) models.
// 5. Why 'Price' is int64_t: Floating point numbers (double/float) have rounding
//    errors (e.g., 0.1 + 0.2 != 0.3). In financial systems, prices are stored
//    in the smallest currency unit (e.g., cents, so $150.25 = 15025 cents).
// ============================================================================

#include <string>
#include <cstdint>  // for uint64_t, int64_t, uint32_t

// Type aliases for readability and domain clarity
using OrderId = uint64_t;   // Unique order identifier (e.g., 1, 2, 3...)
using Price = int64_t;      // Price in cents ($100.50 -> 10050)
using Quantity = uint32_t;  // Number of shares (e.g., 100)

// Side of the order: Buy or Sell
enum class Side {
    Buy,
    Sell
};

// Type of the order:
// - Limit: Buy/Sell at a specific price or better.
// - Market: Execute immediately at the best available price.
enum class OrderType {
    Limit,
    Market
};

// Represents a single order submitted to the exchange
struct Order {
    OrderId id;
    std::string symbol;
    Side side;
    OrderType type;
    Price price;                     // Relevant for Limit orders (0 for Market)
    Quantity initialQuantity;        // Original order size
    Quantity remainingQuantity;      // Unfilled shares waiting in the book

    // Helper constructor to easily create orders
    Order(OrderId id,
          std::string symbol,
          Side side,
          OrderType type,
          Price price,
          Quantity quantity)
        : id(id),
          symbol(std::move(symbol)),
          side(side),
          type(type),
          price(price),
          initialQuantity(quantity),
          remainingQuantity(quantity) {}

    // Check if the order has been completely matched
    bool isFilled() const {
        return remainingQuantity == 0;
    }
};

// Helper function to turn Side into human-readable string
inline std::string toString(Side side) {
    return (side == Side::Buy) ? "BUY" : "SELL";
}

// Helper function to turn OrderType into human-readable string
inline std::string toString(OrderType type) {
    return (type == OrderType::Limit) ? "LIMIT" : "MARKET";
}
