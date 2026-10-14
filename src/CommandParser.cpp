// ============================================================================
// CommandParser.cpp – Implementation of user input parser
// ============================================================================

#include "CommandParser.h"
#include <sstream>
#include <algorithm>
#include <cmath>

// Helper to convert string to uppercase
static std::string toUpper(std::string str) {
    for (char& c : str) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return str;
}

std::optional<Price> CommandParser::parsePriceToCents(const std::string& priceStr) {
    std::string clean = priceStr;
    if (!clean.empty() && clean.front() == '$') {
        clean = clean.substr(1);
    }

    try {
        size_t idx = 0;
        double dollars = std::stod(clean, &idx);
        if (idx != clean.size() || dollars < 0.0) {
            return std::nullopt;
        }
        // Round to nearest cent: e.g. 150.25 -> 15025
        return static_cast<Price>(std::llround(dollars * 100.0));
    } catch (...) {
        return std::nullopt;
    }
}

ParsedCommand CommandParser::parse(const std::string& inputLine) {
    ParsedCommand cmd;
    std::istringstream iss(inputLine);

    std::string action;
    if (!(iss >> action)) {
        return cmd; // Empty input
    }
    action = toUpper(action);

    if (action == "QUIT" || action == "EXIT") {
        cmd.type = CommandType::Quit;
        return cmd;
    }

    if (action == "HELP") {
        cmd.type = CommandType::Help;
        return cmd;
    }

    if (action == "BOOK") {
        cmd.type = CommandType::PrintBook;
        std::string sym;
        if (iss >> sym) {
            cmd.symbol = toUpper(sym);
        }
        return cmd;
    }

    if (action == "LOAD") {
        cmd.type = CommandType::LoadFile;
        std::string path;
        if (iss >> path) {
            cmd.filePath = path;
        } else {
            cmd.type = CommandType::Unknown;
            cmd.errorMessage = "Missing file path. Format: LOAD <filePath>";
        }
        return cmd;
    }

    if (action == "CANCEL") {
        cmd.type = CommandType::CancelOrder;
        OrderId id;
        if (iss >> id) {
            cmd.orderIdToCancel = id;
        } else {
            cmd.type = CommandType::Unknown;
            cmd.errorMessage = "Missing order ID. Format: CANCEL <orderId>";
        }
        return cmd;
    }

    if (action == "BUY" || action == "SELL") {
        Side side = (action == "BUY") ? Side::Buy : Side::Sell;

        std::string token2;
        if (!(iss >> token2)) {
            cmd.type = CommandType::Unknown;
            cmd.errorMessage = "Incomplete command. Format: " + action + " [SYMBOL] <LIMIT/MARKET> <QTY> [PRICE]";
            return cmd;
        }
        token2 = toUpper(token2);

        std::string symbol = "AAPL";
        std::string typeStr;

        // Check if token2 is the order type or a custom stock symbol
        if (token2 == "LIMIT" || token2 == "MARKET") {
            typeStr = token2;
        } else {
            symbol = token2;
            if (!(iss >> typeStr)) {
                cmd.type = CommandType::Unknown;
                cmd.errorMessage = "Missing order type. Format: " + action + " " + symbol + " <LIMIT/MARKET> <QTY> [PRICE]";
                return cmd;
            }
            typeStr = toUpper(typeStr);
        }

        OrderType orderType;
        if (typeStr == "LIMIT") {
            orderType = OrderType::Limit;
        } else if (typeStr == "MARKET") {
            orderType = OrderType::Market;
        } else {
            cmd.type = CommandType::Unknown;
            cmd.errorMessage = "Unknown order type '" + typeStr + "'. Use LIMIT or MARKET.";
            return cmd;
        }

        Quantity qty = 0;
        if (!(iss >> qty) || qty == 0) {
            cmd.type = CommandType::Unknown;
            cmd.errorMessage = "Invalid or missing quantity. Must be a positive integer.";
            return cmd;
        }

        Price priceInCents = 0;
        if (orderType == OrderType::Limit) {
            std::string priceStr;
            if (!(iss >> priceStr)) {
                cmd.type = CommandType::Unknown;
                cmd.errorMessage = "Missing price for LIMIT order. Format: " + action + " ... LIMIT " + std::to_string(qty) + " <PRICE>";
                return cmd;
            }
            auto parsedPrice = parsePriceToCents(priceStr);
            if (!parsedPrice.has_value() || *parsedPrice <= 0) {
                cmd.type = CommandType::Unknown;
                cmd.errorMessage = "Invalid price '" + priceStr + "'. Example: 150.25";
                return cmd;
            }
            priceInCents = *parsedPrice;
        }

        cmd.type = CommandType::AddOrder;
        cmd.symbol = symbol;
        cmd.order = Order(nextOrderId_++, symbol, side, orderType, priceInCents, qty);
        return cmd;
    }

    cmd.type = CommandType::Unknown;
    cmd.errorMessage = "Unrecognized command '" + action + "'. Type HELP for available commands.";
    return cmd;
}
