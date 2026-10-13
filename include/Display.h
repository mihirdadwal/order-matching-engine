#pragma once

// ============================================================================
// Display.h – Colored ANSI terminal rendering of Order Books and Market Stats
// ============================================================================

#include "OrderBook.h"
#include "MarketStats.h"

class Display {
public:
    // Prints the colored order book and market statistics header
    static void printOrderBook(const OrderBook& book, const MarketStats& stats);

    // Prints a trade execution alert in high-contrast color
    static void printTrade(const Trade& trade);
};
