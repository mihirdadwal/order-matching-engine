// main.cpp – program entry point
// Milestone 2 check: Test creating Order and Trade objects.

#include <iostream>
#include "Order.h"
#include "Trade.h"

int main() {
    std::cout << "==========================================\n";
    std::cout << "  Order Matching Engine - Data Model Demo \n";
    std::cout << "==========================================\n";

    // 1. Create a Buy Limit Order ($150.25 for 100 shares of AAPL)
    Order buyOrder(1, "AAPL", Side::Buy, OrderType::Limit, 15025, 100);

    std::cout << "Created Order:\n"
              << "  ID:       #" << buyOrder.id << "\n"
              << "  Symbol:   " << buyOrder.symbol << "\n"
              << "  Side:     " << toString(buyOrder.side) << "\n"
              << "  Type:     " << toString(buyOrder.type) << "\n"
              << "  Price:    " << buyOrder.price << " cents ($" << (buyOrder.price / 100.0) << ")\n"
              << "  Qty:      " << buyOrder.remainingQuantity << " shares\n\n";

    // 2. Simulate matching 40 shares and creating a Trade
    Quantity tradeQty = 40;
    buyOrder.remainingQuantity -= tradeQty;

    Trade trade("AAPL", buyOrder.id, 2 /* seller id */, buyOrder.price, tradeQty);

    std::cout << "Executed Trade:\n"
              << "  Symbol:   " << trade.symbol << "\n"
              << "  Buyer:    #" << trade.buyOrderId << "\n"
              << "  Seller:   #" << trade.sellOrderId << "\n"
              << "  Quantity: " << trade.quantity << " shares\n"
              << "  Price:    " << trade.priceString() << "\n\n";

    std::cout << "Buyer remaining shares: " << buyOrder.remainingQuantity << "\n";
    std::cout << "Is buyer filled? " << (buyOrder.isFilled() ? "Yes" : "No") << "\n";
    std::cout << "==========================================\n";

    return 0;
}
