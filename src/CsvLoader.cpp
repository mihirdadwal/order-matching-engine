// ============================================================================
// CsvLoader.cpp – Implementation of CSV File Replayer
// ============================================================================

#include "CsvLoader.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>
#include <algorithm>

static std::string trim(std::string str) {
    while (!str.empty() && (str.back() == '\r' || str.back() == ' ' || str.back() == '\t')) {
        str.pop_back();
    }
    size_t start = 0;
    while (start < str.size() && (str[start] == ' ' || str[start] == '\t')) {
        start++;
    }
    return str.substr(start);
}

static std::string toUpper(std::string str) {
    for (char& c : str) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return str;
}

LoadResult CsvLoader::loadAndExecute(const std::string& filePath,
                                     Exchange& exchange,
                                     OrderId& nextOrderId) {
    LoadResult res;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        res.success = false;
        res.errorMessage = "Failed to open file: " + filePath;
        return res;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    std::string line;
    bool isFirstLine = true;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        // Skip CSV header row
        if (isFirstLine) {
            isFirstLine = false;
            std::string headerCheck = toUpper(line);
            if (headerCheck.rfind("SIDE", 0) == 0) {
                continue;
            }
        }

        std::stringstream ss(line);
        std::string sideStr, symbolStr, typeStr, qtyStr, priceStr;

        if (!std::getline(ss, sideStr, ',') ||
            !std::getline(ss, symbolStr, ',') ||
            !std::getline(ss, typeStr, ',') ||
            !std::getline(ss, qtyStr, ',')) {
            continue; // Malformed row
        }

        // Price might be empty for MARKET orders
        std::getline(ss, priceStr, ',');

        sideStr = toUpper(trim(sideStr));
        symbolStr = toUpper(trim(symbolStr));
        typeStr = toUpper(trim(typeStr));
        qtyStr = trim(qtyStr);
        priceStr = trim(priceStr);

        Side side = (sideStr == "BUY") ? Side::Buy : Side::Sell;
        OrderType orderType = (typeStr == "MARKET") ? OrderType::Market : OrderType::Limit;

        Quantity qty = 0;
        try {
            qty = static_cast<Quantity>(std::stoul(qtyStr));
        } catch (...) {
            continue;
        }
        if (qty == 0) continue;

        Price priceCents = 0;
        if (orderType == OrderType::Limit && !priceStr.empty()) {
            if (priceStr.front() == '$') priceStr = priceStr.substr(1);
            try {
                double dollars = std::stod(priceStr);
                priceCents = static_cast<Price>(std::llround(dollars * 100.0));
            } catch (...) {
                continue;
            }
        }

        Order order(nextOrderId++, symbolStr, side, orderType, priceCents, qty);
        auto trades = exchange.submitOrder(order);

        res.ordersLoaded++;
        res.tradesExecuted += trades.size();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    res.elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    res.success = true;

    return res;
}
