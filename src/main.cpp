// main.cpp – Demonstrates OrderBook matching, partial fills, and cancellation

#include <iostream>
#include "OrderBook.h"

int main() {
    std::cout << "========================================================\n";
    std::cout << "        ORDER MATCHING ENGINE - LIVE SIMULATION         \n";
    std::cout << "========================================================\n";

    OrderBook book("AAPL");

    // 1. Place initial resting Limit Orders on the book
    std::cout << "\n[Step 1] Seeding order book with initial limit orders...\n";
    book.addOrder(Order(1, "AAPL", Side::Buy,  OrderType::Limit, 15000, 100)); // Buy 100 @ $150.00
    book.addOrder(Order(2, "AAPL", Side::Buy,  OrderType::Limit, 14950, 200)); // Buy 200 @ $149.50
    book.addOrder(Order(3, "AAPL", Side::Sell, OrderType::Limit, 15100,  50)); // Sell 50 @ $151.00
    book.addOrder(Order(4, "AAPL", Side::Sell, OrderType::Limit, 15150, 150)); // Sell 150 @ $151.50

    book.printBook();

    // 2. Incoming aggressive SELL order that partially matches resting Buy #1
    std::cout << "[Step 2] Incoming SELL LIMIT 40 @ $150.00 (Order #5)...\n";
    auto trades = book.addOrder(Order(5, "AAPL", Side::Sell, OrderType::Limit, 15000, 40));

    for (const auto& trade : trades) {
        std::cout << "  -> EXECUTION: " << trade.quantity << " shares @ "
                  << trade.priceString() << " (Buyer #" << trade.buyOrderId
                  << ", Seller #" << trade.sellOrderId << ")\n";
    }

    book.printBook();

    // 3. Incoming MARKET BUY order (sweeps the best ask of 50 shares @ $151.00 + 10 shares @ $151.50)
    std::cout << "[Step 3] Incoming BUY MARKET 60 shares (Order #6)...\n";
    trades = book.addOrder(Order(6, "AAPL", Side::Buy, OrderType::Market, 0, 60));

    for (const auto& trade : trades) {
        std::cout << "  -> EXECUTION: " << trade.quantity << " shares @ "
                  << trade.priceString() << " (Buyer #" << trade.buyOrderId
                  << ", Seller #" << trade.sellOrderId << ")\n";
    }

    book.printBook();

    // 4. Cancel Order #2 (the resting buy of 200 @ $149.50)
    std::cout << "[Step 4] Cancelling Order #2...\n";
    bool cancelled = book.cancelOrder(2);
    std::cout << "  -> Cancel successful? " << (cancelled ? "YES" : "NO") << "\n";

    book.printBook();

    std::cout << "Simulation completed successfully!\n";
    return 0;
}
