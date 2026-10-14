#pragma once

// ============================================================================
// CommandParser.h – Translates user terminal text into structured commands
//
// LEARNING NOTES (C++ vs Java):
// 1. std::istringstream: Like Java's Scanner or StringTokenizer. Reads tokens
//    (words) from a std::string separated by whitespace.
// 2. std::optional: Encapsulates optional fields (e.g. orderIdToCancel is only
//    present for CANCEL commands).
// ============================================================================

#include "Order.h"
#include <string>
#include <optional>

enum class CommandType {
    AddOrder,       // e.g. "BUY LIMIT 100 150.00" or "BUY AAPL LIMIT 100 150.00"
    CancelOrder,    // e.g. "CANCEL 2"
    PrintBook,      // e.g. "BOOK" or "BOOK AAPL"
    LoadFile,       // e.g. "LOAD data/sample_orders.csv"
    Help,           // e.g. "HELP"
    Quit,           // e.g. "QUIT" or "EXIT"
    Unknown         // Unrecognized or malformed command
};

struct ParsedCommand {
    CommandType type{CommandType::Unknown};
    std::optional<Order> order;
    std::optional<OrderId> orderIdToCancel;
    std::string symbol{"AAPL"};
    std::string filePath;
    std::string errorMessage;
};

class CommandParser {
public:
    CommandParser() = default;

    // Parses a single line of terminal input
    ParsedCommand parse(const std::string& inputLine);

    // Resets order ID counter (useful for testing)
    void resetOrderIdCounter() { nextOrderId_ = 1; }
    OrderId& getNextOrderIdRef() { return nextOrderId_; }

private:
    OrderId nextOrderId_{1};

    // Helper to parse price string "$150.25" or "150.25" to cents (15025)
    static std::optional<Price> parsePriceToCents(const std::string& priceStr);
};
